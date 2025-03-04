//
// Created by ubuntu on 1/7/25.
//
#ifndef VECTORFHEMANAGER_H
#define VECTORFHEMANAGER_H
#include "TFHEBool.h"
#include "tfhe_geos/geom/TFHEGeometry.h"

namespace SpatialFHE {


class VectorFHEManager {
public:
    // Predicates
    // virtual TFHEBool disjoint(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool contains(const geom::TFHEGeometry &g, const geom::TFHEGeometry &p) = 0;
    // virtual TFHEBool intersects(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool within(const geom::TFHEGeometry &p, const geom::TFHEGeometry &g) = 0;
    // virtual TFHEBool covers(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool coveredby(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool crosses(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool overlaps(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool touches(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;
    // virtual TFHEBool equals(const geom::TFHEGeometry &g1, const geom::TFHEGeometry &g2) = 0;

    VectorFHEManager();
    VectorFHEManager(const std::string &publicKeyPath, const std::string &secretKeyPath, const std::string &server_url, bool isInit);
    ~VectorFHEManager();

private:
    TFHEContext *context;

};

} // SpatialFHE

#endif //VECTORFHEMANAGER_H
