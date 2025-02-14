/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: io/WKTReader.java rev. 1.1 (JTS-1.7)
 *
 **********************************************************************/

#pragma once

#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHEGeometryFactory.h>

#include <string>

// Forward declarations
namespace SpatialFHE {
    namespace io {
        class StringTokenizer;
    }

    namespace geom {
        class TFHECoordinate;
        class TFHECoordinateSequence;
        class TFHEPoint;
        class TFHELineString;
        class TFHELinearRing;
        class TFHEPolygon;
    }  // namespace geom
}  // namespace SpatialFHE

namespace SpatialFHE::io {

    /**
     * \class WKTReader
     * \brief WKT parser class; see also WKTWriter.
     */
    class WKTReader {
    public:
        /**
         * \brief Initialize parser with given GeometryFactory.
         *
         * Note that all Geometry objects created by the
         * parser will contain a pointer to the given factory
         * so be sure you'll keep the factory alive for the
         * whole WKTReader and created Geometry life.
         */

        explicit WKTReader(const geom::TFHEGeometryFactory* gf) : geometryFactory(gf), fixStructure(false){};

        /**
         * \brief Initialize parser with default GeometryFactory.
         *
         */
        WKTReader() : geometryFactory(geom::TFHEGeometryFactory::getDefaultInstance()), fixStructure(false){};

        ~WKTReader(){};

        void setFixStructure(bool doFixStructure) {
            fixStructure = doFixStructure;
        }

        /// Parse a WKT string returning a Geometry
        template <typename T> std::unique_ptr<T> read(const std::string& wkt) const {
            auto g = read(wkt);
            auto gt = dynamic_cast<const T*>(g.get());
            if (!gt) {
                // Can improve this message once there's a good way to get a string repr of T
                throw std::runtime_error("Unexpected WKT type");
            }
            return std::unique_ptr<T>(static_cast<T*>(g.release()));
        }

        std::unique_ptr<geom::TFHEGeometry> read(const std::string& wellKnownText) const;

    protected:
        std::unique_ptr<geom::TFHECoordinateSequence> getCoordinates(io::StringTokenizer* tokenizer) const;
        static double getNextNumber(io::StringTokenizer* tokenizer);
        static std::string getNextEmptyOrOpener(io::StringTokenizer* tokenizer);
        static std::string getNextCloserOrComma(io::StringTokenizer* tokenizer);
        static std::string getNextCloser(io::StringTokenizer* tokenizer);
        static std::string getNextWord(io::StringTokenizer* tokenizer);
        std::unique_ptr<geom::TFHEGeometry> readGeometryTaggedText(
            io::StringTokenizer* tokenizer,
            const geom::TFHEGeometryTypeId* emptyType = nullptr) const;

        std::unique_ptr<geom::TFHEPoint> readPointText(io::StringTokenizer* tokenizer) const;
        std::unique_ptr<geom::TFHELineString> readLineStringText(io::StringTokenizer* tokenizer) const;
        std::unique_ptr<geom::TFHELinearRing> readLinearRingText(io::StringTokenizer* tokenizer) const;
        std::unique_ptr<geom::TFHEPolygon> readPolygonText(io::StringTokenizer* tokenizer) const;

        /// Read the contents of a POLYGON or a CURVEPOLYGON
        std::unique_ptr<geom::TFHEGeometry> readSurfaceText(io::StringTokenizer* tokenizer) const;

    private:
        const geom::TFHEGeometryFactory* geometryFactory;
        bool fixStructure;

        void getPreciseCoordinate(io::StringTokenizer* tokenizer, geom::TFHECoordinate&) const;

        static bool isNumberNext(io::StringTokenizer* tokenizer);
        static bool isOpenerNext(io::StringTokenizer* tokenizer);

        static bool isTypeName(const std::string& type, const std::string& typeName);
    };

}  // namespace SpatialFHE::io
