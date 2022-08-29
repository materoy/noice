
use crate::SampleRequestOptions;
use crate::capture::stream_make_input;
use crate::playback::stream_make_output;

pub fn stream_setup_for<F, N>(
    on_input_sample: N,
    on_output_sample: F,
) -> Result<(cpal::Stream, cpal::Stream), anyhow::Error>
where
    F: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static + Copy,
    N: FnMut(&mut SampleRequestOptions) -> f32 + std::marker::Send + 'static + Copy,
{
    let (_host, input_device, input_config) = crate::capture::host_input_device_setup()?;
    let (_host, output_device, output_config) = crate::playback::host_output_device_setup()?;

    let (input_stream, output_stream) = match input_config.sample_format() {
        cpal::SampleFormat::F32 => {
            let (sender, recever) = std::sync::mpsc::channel();

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
            let (sender, recever) = std::sync::mpsc::channel();

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
            let (sender, recever) = std::sync::mpsc::channel();

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
