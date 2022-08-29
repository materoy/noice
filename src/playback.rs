
use cpal::traits::{HostTrait, DeviceTrait};

use std::{
    fmt::Display,
    sync::mpsc::Receiver,
};

use crate::SampleRequestOptions;

pub fn sample_next_output(o: &mut SampleRequestOptions) -> f32 {
    o.tick();
    o.tone(440.) * 0.1 + o.tone(880.) * 0.1
    // combination of several tones
}


pub fn host_output_device_setup(
) -> Result<(cpal::Host, cpal::Device, cpal::SupportedStreamConfig), anyhow::Error> {
    let host = cpal::default_host();

    let device = host
        .default_output_device()
        .ok_or_else(|| anyhow::Error::msg("Default output device is not available"))?;
    println!("Output device : {}", device.name()?);

    let config = device.default_output_config()?;
    println!("Default output config : {:?}", config);

    Ok((host, device, config))
}


pub fn stream_make_output<T, F>(
    device: &cpal::Device,
    config: &cpal::StreamConfig,
    _on_sample: F,
    receiver: Receiver<T>,
) -> Result<cpal::Stream, anyhow::Error>
where
    T: cpal::Sample + std::marker::Send + 'static + Copy + Display,
    F: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static + Copy,
{
    let sample_rate = config.sample_rate.0 as f32;
    let sample_clock = 0f32;
    let nchannels = config.channels as usize;
    let mut _request = SampleRequestOptions {
        sample_rate,
        sample_clock,
        nchannels,
    };
    let err_fn = |err| eprintln!("Error building output sound stream: {}", err);

    let stream = device.build_output_stream(
        config,
        move |output: &mut [T], _: &cpal::OutputCallbackInfo| {
            // on_window(output, &mut request, on_sample, receiver)

            for sample in output.iter_mut() {
                let received = receiver.recv().unwrap();
                *sample = received;
                println!("{}", received);
            }
        },
        err_fn,
    )?;

    Ok(stream)
}
