#ifndef TFHELINESTRING_H
#define TFHELINESTRING_H
#include "TFHECoordinateSequence.h"
#include "TFHEGeometry.h"


namespace SpatialFHE::geom {

    class TFHELineString : public TFHEGeometry {
    protected:
        std::unique_ptr<TFHEEnvelope> computeEnvelopeInternal() const override;
        [[nodiscard]] TFHELineString* cloneInternal() const override;
        TFHELineString(const TFHELineString& other);

        TFHELineString(TFHECoordinateSequence* pts, const TFHEGeometryFactory* factory);
        TFHELineString(TFHECoordinateSequence::Ptr &&pts, const TFHEGeometryFactory& factory);
        TFHELineString(std::vector<TFHECoordinate> &&pts, const TFHEGeometryFactory& factory);

        TFHECoordinateSequence::Ptr points;

    public:
        friend class TFHEGeometryFactory;
        using Ptr = std::unique_ptr<TFHELineString>;

        ~TFHELineString() override = default;

        [[nodiscard]] bool isEmpty() const override;
        [[nodiscard]] const TFHECoordinate *getCoordinate() const override;
        [[nodiscard]] std::unique_ptr<TFHECoordinateSequence> getCoordinates() const override;

        /// Returns a read-only pointer to internal coordinates
        const TFHECoordinateSequence* getCoordinatesRO() const;

        [[nodiscard]] std::size_t getNumPoints() const override;

        [[nodiscard]] std::string getGeometryType() const override;
        [[nodiscard]] TFHEGeometryTypeId getGeometryTypeId() const override;
        [[nodiscard]] Dimension::DimensionType getDimension() const override;

        virtual std::unique_ptr<TFHEPoint> getPointN(std::size_t n) const;
        virtual std::unique_ptr<TFHEPoint> getStartPoint() const;
        virtual std::unique_ptr<TFHEPoint> getEndPoint() const;
        virtual TFHEBool isClosed() const;
        virtual TFHEBool isRing() const;

        [[nodiscard]] Ptr clone() const;

    private:
        void validateConstruction();
    };

} // geom
// SpatialFHE

#endif //TFHELINESTRING_H
