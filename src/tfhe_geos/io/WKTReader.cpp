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

#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/geom/TFHELinearRing.h>
#include <tfhe_geos/geom/TFHEPoint.h>
#include <tfhe_geos/geom/TFHEPolygon.h>
#include <tfhe_geos/io/StringTokenizer.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/util/string.h>

#include <cassert>
#include <sstream>
#include <string>

using namespace SpatialFHE::geom;

namespace SpatialFHE::io {  // geos.io

    std::unique_ptr<TFHEGeometry> WKTReader::read(const std::string& wellKnownText) const {
        StringTokenizer tokenizer(wellKnownText);
        auto ret = readGeometryTaggedText(&tokenizer);

        if (tokenizer.peekNextToken() != StringTokenizer::TT_EOF) {
            tokenizer.nextToken();
            throw std::runtime_error("Unexpected text after end of geometry");
        }

        return ret;
    }

    std::unique_ptr<TFHECoordinateSequence> WKTReader::getCoordinates(StringTokenizer* tokenizer) const {
        std::string nextToken = getNextEmptyOrOpener(tokenizer);
        if (nextToken == "EMPTY") {
            return std::make_unique<TFHECoordinateSequence>(static_cast<size_t>(0));
        }

        TFHECoordinate coord(TFHEInt32(0), TFHEInt32(0));
        getPreciseCoordinate(tokenizer, coord);

        auto coordinates = std::make_unique<TFHECoordinateSequence>(static_cast<size_t>(0));
        coordinates->add(coord);

        nextToken = getNextCloserOrComma(tokenizer);
        while (nextToken == ",") {
            getPreciseCoordinate(tokenizer, coord);
            coordinates->add(coord);
            nextToken = getNextCloserOrComma(tokenizer);
        }

        return coordinates;
    }

    void WKTReader::getPreciseCoordinate(StringTokenizer* tokenizer, TFHECoordinate& coord) const {
        coord.x = TFHEInt32(static_cast<int32_t>(getNextNumber(tokenizer)));
        coord.y = TFHEInt32(static_cast<int32_t>(getNextNumber(tokenizer)));
    }

    bool WKTReader::isNumberNext(StringTokenizer* tokenizer) {
        return tokenizer->peekNextToken() == StringTokenizer::TT_NUMBER;
    }

    bool WKTReader::isOpenerNext(StringTokenizer* tokenizer) {
        return tokenizer->peekNextToken() == '(';
    }

    double WKTReader::getNextNumber(StringTokenizer* tokenizer) {
        int type = tokenizer->nextToken();
        switch (type) {
        case StringTokenizer::TT_EOF:
            throw std::runtime_error("Expected number but encountered end of stream");
        case StringTokenizer::TT_EOL:
            throw std::runtime_error("Expected number but encountered end of line");
        case StringTokenizer::TT_NUMBER:
            return tokenizer->getNVal();
        case StringTokenizer::TT_WORD:
            throw std::runtime_error("Expected number but encountered word: " + tokenizer->getSVal());
        case '(':
            throw std::runtime_error("Expected number but encountered '('");
        case ')':
            throw std::runtime_error("Expected number but encountered ')'");
        case ',':
            throw std::runtime_error("Expected number but encountered ','");
        }
        assert(0);  // Encountered unexpected StreamTokenizer type
        return 0;
    }

    bool WKTReader::isTypeName(const std::string& type, const std::string& typeName) {
        return util::startsWith(type, typeName);
    }

    std::string WKTReader::getNextEmptyOrOpener(StringTokenizer* tokenizer) {
        std::string nextWord = getNextWord(tokenizer);

        bool flagsModified = false;

        if (nextWord == "EMPTY" || nextWord == "(") {
            return nextWord;
        }

        std::ostringstream err;
        err << "Expected 'EMPTY' or '(' but encountered " << nextWord;
        throw std::runtime_error(err.str());
    }

    std::string WKTReader::getNextCloserOrComma(StringTokenizer* tokenizer) {
        std::string nextWord = getNextWord(tokenizer);
        if (nextWord == "," || nextWord == ")") {
            return nextWord;
        }
        std::ostringstream err;
        err << "Expected ')' or ',' but encountered " << nextWord;
        throw std::runtime_error(err.str());
    }

    std::string WKTReader::getNextCloser(StringTokenizer* tokenizer) {
        std::string nextWord = getNextWord(tokenizer);
        if (nextWord == ")") {
            return nextWord;
        }
        std::ostringstream err;
        err << "Expected ')' but encountered " << nextWord;
        throw std::runtime_error(err.str());
    }

    std::string WKTReader::getNextWord(StringTokenizer* tokenizer) {
        int type = tokenizer->nextToken();
        switch (type) {
        case StringTokenizer::TT_EOF:
            throw std::runtime_error("Expected word but encountered end of stream");
        case StringTokenizer::TT_EOL:
            throw std::runtime_error("Expected word but encountered end of line");
        case StringTokenizer::TT_NUMBER:
            throw std::runtime_error("Expected word but encountered number: " + std::to_string(tokenizer->getNVal()));
        case StringTokenizer::TT_WORD: {
            std::string word = tokenizer->getSVal();
            for (char& c : word) {
                // Avoid UB if c is not representable as unsigned char
                // https://en.cppreference.com/w/cpp/string/byte/toupper
                c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
            }
            return word;
        }
        case '(':
            return "(";
        case ')':
            return ")";
        case ',':
            return ",";
        }
        assert(0);
        // throw  ParseException("Encountered unexpected StreamTokenizer type");
        return "";
    }

    std::unique_ptr<TFHEGeometry> WKTReader::readGeometryTaggedText(
        StringTokenizer* tokenizer,
        const TFHEGeometryTypeId* emptyType) const {
        std::string type = getNextWord(tokenizer);

        std::unique_ptr<TFHEGeometry> geom;

        if (isTypeName(type, "POINT")) {
            geom = readPointText(tokenizer);
        } else if (isTypeName(type, "LINESTRING")) {
            geom = readLineStringText(tokenizer);
        } else if (isTypeName(type, "LINEARRING")) {
            geom = readLinearRingText(tokenizer);
        } else if (isTypeName(type, "POLYGON")) {
            geom = readPolygonText(tokenizer);
        } else if (type == "EMPTY" && emptyType != nullptr) {
            return geometryFactory->createEmptyGeometry(*emptyType);
        } else {
            std::ostringstream err;
            err << "Unknown type: " << type;
            throw std::runtime_error(err.str());
        }

        return geom;
    }

    std::unique_ptr<TFHEPoint> WKTReader::readPointText(StringTokenizer* tokenizer) const {
        auto&& coords = getCoordinates(tokenizer);
        return geometryFactory->createPoint(std::move(coords));
    }

    std::unique_ptr<TFHELineString> WKTReader::readLineStringText(StringTokenizer* tokenizer) const {
        auto&& coords = getCoordinates(tokenizer);
        return geometryFactory->createLineString(std::move(coords));
    }

    std::unique_ptr<TFHELinearRing> WKTReader::readLinearRingText(StringTokenizer* tokenizer) const {
        auto&& coords = getCoordinates(tokenizer);
        if (fixStructure) {
            coords->closeRing();
        }
        return geometryFactory->createLinearRing(std::move(coords));
    }

    std::unique_ptr<TFHEGeometry> WKTReader::readSurfaceText(StringTokenizer* tokenizer) const {
        int type = tokenizer->peekNextToken();
        if (type == '(') {
            return readPolygonText(tokenizer);
        }

        TFHEGeometryTypeId defaultType = TFHEGeometryTypeId::TFHE_POLYGON;
        auto component = readGeometryTaggedText(tokenizer, &defaultType);
        if (dynamic_cast<TFHEPolygon*>(component.get())) {
            return component;
        }

        throw std::runtime_error("Expected POLYGON but got " + component->getGeometryType());
    }

    std::unique_ptr<TFHEPolygon> WKTReader::readPolygonText(StringTokenizer* tokenizer) const {
        std::string nextToken = getNextEmptyOrOpener(tokenizer);
        if (nextToken == "EMPTY") {
            auto coords = std::make_unique<TFHECoordinateSequence>(static_cast<size_t>(0));
            auto ring = geometryFactory->createLinearRing(std::move(coords));
            return geometryFactory->createPolygon(std::move(ring));
        }

        std::vector<std::unique_ptr<TFHELinearRing>> holes;
        auto shell = readLinearRingText(tokenizer);
        nextToken = getNextCloserOrComma(tokenizer);
        while (nextToken == ",") {
            holes.push_back(readLinearRingText(tokenizer));
            nextToken = getNextCloserOrComma(tokenizer);
        }

        return geometryFactory->createPolygon(std::move(shell), std::move(holes));
    }

}  // namespace SpatialFHE::io
