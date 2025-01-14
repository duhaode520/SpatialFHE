#ifndef TFHEPOINT_H
#define TFHEPOINT_H
#include "TFHECoordinateSequence.h"
#include "TFHEGeometry.h"

namespace SpatialFHE::geom {

    // Forward declaration
    class TFHEGeometryFactory;

    class TFHEPoint : public TFHEGeometry {
    private:
        TFHECoordinateSequence coordinates;

    protected:
        std::unique_ptr<TFHEEnvelope> computeEnvelopeInternal() const override;
        [[nodiscard]] TFHEPoint *cloneInternal() const override;

        TFHEPoint(const TFHECoordinate &c, const TFHEGeometryFactory *newFactory);
        TFHEPoint(TFHECoordinateSequence &&c, const TFHEGeometryFactory *newFactory);
        TFHEPoint(TFHECoordinateSequence *coords, const TFHEGeometryFactory *newFactory);
        TFHEPoint(const TFHEPoint &p);

    public:
        friend class TFHEGeometryFactory;
        using Ptr = std::unique_ptr<TFHEPoint>;

        ~TFHEPoint() override = default;

        bool isEmpty() const override;

        [[nodiscard]] const TFHECoordinate *getCoordinate() const override;
        [[nodiscard]] std::unique_ptr<TFHECoordinateSequence> getCoordinates() const override;

        [[nodiscard]] std::size_t getNumPoints() const override;

        [[nodiscard]] std::string getGeometryType() const override;

        [[nodiscard]] TFHEGeometryTypeId getGeometryTypeId() const override;

        [[nodiscard]] Dimension::DimensionType getDimension() const override;

        void setXY(TFHEInt32 x, TFHEInt32 y);

        [[nodiscard]] TFHEInt32 getX() const;
        [[nodiscard]] TFHEInt32 getY() const;

        [[nodiscard]] Ptr clone() const;
    };

}  // namespace SpatialFHE::geom

#endif  // TFHEPOINT_H
