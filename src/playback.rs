use std::fmt::Debug;

use cpal::traits::{DeviceTrait, HostTrait};

use flume::Receiver;

use crate::types::{Msg, SampleSize};

pub struct AudioPlayback {
    device: cpal::Device,
    supported_config: cpal::SupportedStreamConfig,
}
impl AudioPlayback {
    pub fn new() -> anyhow::Result<Self, String> {
        let (supported_config, device) = AudioPlayback::get_stream_config()?;
        println!("Output config: {:?}", supported_config);
        Ok(Self {
            device,
            supported_config,
        })
    }
    fn get_device() -> anyhow::Result<cpal::Device, String> {
        let host = cpal::default_host();
        host.default_output_device()
            .ok_or_else(|| "No output device".into())
    }
    fn get_stream_config() -> anyhow::Result<(cpal::SupportedStreamConfig, cpal::Device), String> {
        let device = AudioPlayback::get_device()?;
        let config_range = device.supported_output_configs();
        let config_range = config_range
            .map_err(|_| "No config range".to_string())?
            .next()
            .ok_or_else(|| "No supported stream configs".to_string())?;
        let supported_config = config_range.with_max_sample_rate();

        Ok((supported_config, device))
    }
    pub fn play(self, rx: Receiver<Msg>) -> anyhow::Result<cpal::Stream, String> {
        match self.supported_config.sample_format() {
            cpal::SampleFormat::I16 => self.play_type::<i16>(rx),
            cpal::SampleFormat::U16 => self.play_type::<u16>(rx),
            cpal::SampleFormat::F32 => self.play_type::<f32>(rx),
        }
        .map_err(|err| err.to_string())
    }
    fn play_type<T: cpal::Sample + Send + Sync + 'static + Debug>(
        self,
        rx: Receiver<Msg>,
    ) -> anyhow::Result<cpal::Stream, cpal::BuildStreamError> {
        self.device.build_output_stream(
            &self.supported_config.config(),
            move |buffer: &mut [T], _| {
                let samples = match rx.try_recv() {
                    Ok(packets) => packets.into_iter().map::<T, _>(|p| match p {
                        SampleSize::I16(v) => cpal::Sample::from(&v),
                        SampleSize::U16(v) => cpal::Sample::from(&v),
                        SampleSize::F32(v) => cpal::Sample::from(&v),
                    }),
                    _ => {
                        return buffer.iter_mut().for_each(|buf| {
                            *buf = cpal::Sample::from(&0.0);
                        });
                    }
                };
                samples.zip(buffer.iter_mut()).for_each(|(sample, buf)| {
                    let inv_sample: T = match self.supported_config.sample_format() {
                        cpal::SampleFormat::I16 => cpal::Sample::from(&(sample.to_i16() * -1)),
                        cpal::SampleFormat::U16 => cpal::Sample::from(&(sample.to_u16() * 1)),
                        cpal::SampleFormat::F32 => cpal::Sample::from(&(sample.to_f32() * -1.0)),
                    };

                    *buf = inv_sample;
                });
            },
            |_| {},
        )
    }
}
