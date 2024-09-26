package org.ade.SpatialFHE;

import org.ade.SpatialFHE.spatialfhe.*;

import java.nio.file.Path;
import java.nio.file.Paths;

public class LoadTest {
    public static void main(String[] args) {
        FHEHelper helper = FHEHelper.getOrCreate("hdfs://dhdmaster:8020/tmp/public.key", "tmp/private.key", true);
        SEALCrypto crypto = (SEALCrypto)helper.getCrypto();
        PlainText pt1 = crypto.Encode(1.0);
        PlainText pt2 = crypto.Encode(2.0);
        CipherText ct1 = crypto.Encrypt(pt1);
        CipherText ct2 = crypto.Encrypt(pt2);
        CipherText ct3 = crypto.Add(ct1, ct2);
        PlainText pt3 = crypto.Decrypt(ct3);
        DoubleVector dv = new DoubleVector();
        crypto.Decode(dv, pt3);
        System.out.println(dv.get(0));
    }
}
