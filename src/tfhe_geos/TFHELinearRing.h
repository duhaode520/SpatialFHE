//
// Created by ubuntu on 1/13/25.
//

#ifndef TFHELINEARRING_H
#define TFHELINEARRING_H

#include "tfhe_geos/TFHELineString.h"

namespace SpatialFHE::geom {

    class TFHELinearRing : public TFHELineString {
    private:
        void validateConstruction();
    protected:
        TFHELinearRing* cloneInternal() const override;
    public:
        static const size_t MINIMUM_VALID_SIZE = 3;
        TFHELinearRing(const TFHELinearRing &other);
        TFHELinearRing(TFHECoordinateSequence* points, const TFHEGeometryFactory* factory);
        TFHELinearRing(TFHECoordinateSequence::Ptr&& points, const TFHEGeometryFactory& factory);
        TFHELinearRing(std::vector<TFHECoordinate>&& pts, const TFHEGeometryFactory& factory);

        [[nodiscard]] std::unique_ptr<TFHELinearRing> clone() const;

        ~TFHELinearRing() override = default;

        TFHEBool isClosed() const override;

        std::string getGeometryType() const override;
        TFHEGeometryTypeId getGeometryTypeId() const override;

        void setPoints(const TFHECoordinateSequence* points);
    };

} // geom
// SpatialFHE

#endif //TFHELINEARRING_H
