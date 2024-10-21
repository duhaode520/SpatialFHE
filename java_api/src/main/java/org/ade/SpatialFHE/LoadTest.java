package org.ade.SpatialFHE;

import org.ade.SpatialFHE.spatialfhe.*;

public class LoadTest {
    public static void main(String[] args) {
        FHEHelper helper = FHEHelper.getOrCreate("tmp/public.key", "tmp/private.key", "conf", true);
        SpatialFHEManager manager = helper.getManager();
        CipherText ct1 = manager.encodeAndEncrypt(1.0);
        CipherText ct2 = manager.encodeAndEncrypt(2.0);
        CipherText ct3 = manager.add(ct1, ct2);
        DoubleVector dv = new DoubleVector();
        manager.decodeAndDecrypt(dv, ct3);
        System.out.println(dv.get(0));
    }
}
