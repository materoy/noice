

#[derive(Debug)]
pub enum SampleSize {
    I16(i16),
    U16(u16),
    F32(f32),
}

pub type Msg = Vec<SampleSize>;
