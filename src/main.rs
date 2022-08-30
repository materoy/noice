

mod playback;
mod capture;
mod types;

use std::sync::{Mutex, Arc};

use capture::AudioCapture;
use cpal::traits::StreamTrait;
use playback::AudioPlayback;
use types::Msg;

fn main() -> anyhow::Result<(), String> {
    let (tx, rx) = flume::bounded::<Msg>(1);
    let is_paused = Arc::new(Mutex::new(false));

    // rx will be dropped without variable
    let playback = AudioPlayback::new()?.play(rx)?;
    let capture = AudioCapture::new()?.listen(tx, is_paused.clone())?;
    capture.play().unwrap();
    playback.play().unwrap();

    loop {
        // *is_paused.lock().unwrap() = true;
        // thread::sleep(Duration::from_secs(1));
        *is_paused.lock().unwrap() = false;
        std::thread::sleep(std::time::Duration::from_millis(5000));
    }
    // std::thread::sleep(std::time::Duration::from_millis(3000));
    // Ok(())
}


