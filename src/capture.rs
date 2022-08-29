use cpal::traits::{DeviceTrait, HostTrait};

use crate::SampleRequestOptions;

pub fn sample_next_input(o: &mut SampleRequestOptions) -> f32 {
    o.tick();
    o.tone(440.) * 0.1 + o.tone(880.) * 0.1
    // combination of several tones
}

pub fn host_input_device_setup(
) -> Result<(cpal::Host, cpal::Device, cpal::SupportedStreamConfig), anyhow::Error> {
    let host = cpal::default_host();

    let device = host
        .default_input_device()
        .ok_or_else(|| anyhow::Error::msg("Default input device is not available"))?;
    println!("Input device : {}", device.name()?);

    let config = device.default_input_config()?;
    println!("Default input config : {:?}", config);

    Ok((host, device, config))
}

pub fn stream_make_input<T, F>(
    device: &cpal::Device,
    config: &cpal::StreamConfig,
    _on_sample: F,
    sender: std::sync::mpsc::Sender<T>,
) -> Result<cpal::Stream, anyhow::Error>
where
    T: cpal::Sample + std::marker::Send + 'static + Copy,
    F: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static + Copy,
{
    let _sample_rate = config.sample_rate.0 as f32;
    let _sample_clock = 0f32;
    let nchannels = config.channels as usize;

    let err_fn = |err| eprintln!("Error building output sound stream: {}", err);

    let stream = device.build_input_stream(
        config,
        move |data: &[T], _: &cpal::InputCallbackInfo| {
            for chunk in data.chunks(nchannels) {
                for sample in chunk.iter() {
                    sender.send(*sample).unwrap();
                }
            }
        },
        err_fn,
    )?;

    Ok(stream)
}
