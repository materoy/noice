use std::usize;

use anyhow::Ok;
use cpal::{
    traits::{DeviceTrait, HostTrait},
    InputCallbackInfo, SampleFormat, StreamConfig, Sample,
};

extern crate anyhow;
extern crate clap;
extern crate cpal;

const _BUFFSIZE: usize = 1024; // Affects latency
const _FREQ: usize = 2250;

fn main() -> anyhow::Result<()> {
    let host = cpal::default_host();

    let input_device = host
        .default_input_device()
        .ok_or_else(|| anyhow::Error::msg("Default input device is not available"))?;
    let _output_device = host
        .default_output_device()
        .ok_or_else(|| anyhow::Error::msg("Default output device is not available"))?;

    let input_config = input_device.default_input_config()?;

    let config = StreamConfig {
        channels: input_config.channels(),
        sample_rate: input_config.sample_rate(),
        buffer_size: cpal::BufferSize::Default,
    };

    let err_fn = |err| eprintln!("Error building output stream: {}", err);

    match input_config.sample_format() {
        SampleFormat::I16 => {
            input_device.build_input_stream(&config, handle_stream::<i16>, err_fn)?;
        }
        SampleFormat::U16 => {
            input_device.build_input_stream(&config, handle_stream::<u16>, err_fn)?;
        },
        SampleFormat::F32 => {
            input_device.build_input_stream(&config, handle_stream::<f32>, err_fn)?;
        },
    };

    std::thread::sleep(std::time::Duration::from_millis(3000));
    Ok(())
}

fn handle_stream<T>(_data: &[T], callback_info: &InputCallbackInfo) 
where
    T: Sample
{
 
    println!("{:?}", callback_info);
}
