extern crate anyhow;
extern crate clap;
extern crate cpal;

use std::{
    fmt::Display,
    sync::mpsc::{channel, Receiver, Sender},
};

use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};

fn main() -> anyhow::Result<()> {
    let stream = stream_setup_for(sample_next_input, sample_next_output)?;
    stream.0.play()?;
    stream.1.play()?;

    std::thread::sleep(std::time::Duration::from_millis(3000));
    Ok(())
}

fn sample_next_input(o: &mut SampleRequestOptions) -> f32 {
    o.tick();
    o.tone(440.) * 0.1 + o.tone(880.) * 0.1
    // combination of several tones
}

fn sample_next_output(o: &mut SampleRequestOptions) -> f32 {
    o.tick();
    o.tone(440.) * 0.1 + o.tone(880.) * 0.1
    // combination of several tones
}

pub struct SampleRequestOptions {
    pub sample_rate: f32,
    pub sample_clock: f32,
    pub nchannels: usize,
}

impl SampleRequestOptions {
    fn tone(&self, freq: f32) -> f32 {
        (self.sample_clock * freq * 2.0 * std::f32::consts::PI / self.sample_rate).sin()
    }
    fn tick(&mut self) {
        self.sample_clock = (self.sample_clock + 1.0) % self.sample_rate;
    }
}

pub fn stream_setup_for<F, N>(
    on_input_sample: N,
    on_output_sample: F,
) -> Result<(cpal::Stream, cpal::Stream), anyhow::Error>
where
    F: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static + Copy,
    N: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static + Copy,
{
    let (_host, input_device, input_config) = host_input_device_setup()?;
    let (_host, output_device, output_config) = host_output_device_setup()?;

    let (input_stream, output_stream) = match input_config.sample_format() {
        cpal::SampleFormat::F32 => {
            let (sender, recever) = channel();

            let input_stream = stream_make_input::<f32, _>(
                &input_device,
                &input_config.into(),
                on_input_sample,
                sender,
            )?;

            let output_stream = stream_make_output::<f32, _>(
                &output_device,
                &output_config.into(),
                on_output_sample,
                recever,
            )?;
            (input_stream, output_stream)
        }

        cpal::SampleFormat::I16 => {
            let (sender, recever) = channel();

            let input_stream = stream_make_input::<i16, _>(
                &input_device,
                &input_config.into(),
                on_input_sample,
                sender,
            )?;

            let output_stream = stream_make_output::<i16, _>(
                &output_device,
                &output_config.into(),
                on_output_sample,
                recever,
            )?;
            (input_stream, output_stream)
        }

        cpal::SampleFormat::U16 => {
            let (sender, recever) = channel();

            let input_stream = stream_make_input::<u16, _>(
                &input_device,
                &input_config.into(),
                on_input_sample,
                sender,
            )?;

            let output_stream = stream_make_output::<u16, _>(
                &output_device,
                &output_config.into(),
                on_output_sample,
                recever,
            )?;
            (input_stream, output_stream)
        }
    };

    Ok((input_stream, output_stream))
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

pub fn host_input_device_setup(
) -> Result<(cpal::Host, cpal::Device, cpal::SupportedStreamConfig), anyhow::Error> {
    let host = cpal::default_host();

    let device = host
        .default_input_device()
        .ok_or_else(|| anyhow::Error::msg("Default input device is not available"))?;
    println!("Input device : {}", device.name()?);

    let config = device.default_output_config()?;
    println!("Default input config : {:?}", config);

    Ok((host, device, config))
}

pub fn stream_make_input<T, F>(
    device: &cpal::Device,
    config: &cpal::StreamConfig,
    _on_sample: F,
    sender: Sender<T>,
) -> Result<cpal::Stream, anyhow::Error>
where
    T: cpal::Sample + std::marker::Send + 'static + Copy,
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

    let stream = device.build_input_stream(
        config,
        move |data: &[T], _: &cpal::InputCallbackInfo| {
            for sample in data.iter() {
                sender.send(*sample).unwrap();
            }
        },
        err_fn,
    )?;

    Ok(stream)
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

fn _on_window<T, F>(output: &mut [T], request: &mut SampleRequestOptions, mut on_sample: F)
where
    T: cpal::Sample + std::marker::Send + 'static + Copy + Display,
    F: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static,
{
    for frame in output.chunks_mut(request.nchannels) {
        let value: T = cpal::Sample::from::<f32>(&on_sample(request));
        for sample in frame.iter_mut() {
            *sample = value;
        }
    }
}
