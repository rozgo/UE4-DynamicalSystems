#![allow(dead_code)]
use std::num::Wrapping as w;

#[derive(Copy, Clone)]
pub struct Rnd(u64);

impl Rnd {

    #[inline]
    pub fn next(&mut self) -> u64 {
        let mut z = w(self.0) + w(0x9E3779B97F4A7C15_u64);
        self.0 = z.0;
        z = (z ^ (z >> 30)) * w(0xBF58476D1CE4E5B9_u64);
        z = (z ^ (z >> 27)) * w(0x94D049BB133111EB_u64);
        (z ^ (z >> 31)).0
    }
}

pub fn new(seed : u64) -> Rnd {
    Rnd(seed)
}
