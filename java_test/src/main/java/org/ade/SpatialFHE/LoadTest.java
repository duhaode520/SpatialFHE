package org.ade.SpatialFHE;
import org.ade.SpatialFHE.spatialfhe.SEALCrypto;

public class LoadTest {
    public static void main(String[] args) {
        SEALCrypto seal = new SEALCrypto();
        seal.GenerateKeys();
    }
}
