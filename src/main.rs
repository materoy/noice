use cpal::traits::StreamTrait;

mod playback;
mod capture;
mod stream;

fn main() -> anyhow::Result<()> {
    let stream = stream::stream_setup_for(capture::sample_next_input, playback::sample_next_output)?;
    stream.0.play()?;
    stream.1.play()?;

    std::thread::sleep(std::time::Duration::from_millis(3000));
    Ok(())
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




fn _on_window<T, F>(output: &mut [T], request: &mut SampleRequestOptions, mut on_sample: F)
where
    T: cpal::Sample + std::marker::Send + 'static + Copy + std::fmt::Display,
    F: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static,
{
    for frame in output.chunks_mut(request.nchannels) {
        let value: T = cpal::Sample::from::<f32>(&on_sample(request));
        for sample in frame.iter_mut() {
            *sample = value;
        }
    }
}
