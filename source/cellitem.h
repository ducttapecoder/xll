#pragma once

#include <xlw/xlw.h>

using QFCellValue = xlw::impl::MJCellValue;

const auto EXCEL_ERROR_DIV_0 = xlw::CellValue::error_type{7};
const auto EXCEL_ERROR_NA = xlw::CellValue::error_type{42};
const auto EXCEL_ERROR_NAME = xlw::CellValue::error_type{29};
const auto EXCEL_ERROR_NULL = xlw::CellValue::error_type{0};
const auto EXCEL_ERROR_NUM = xlw::CellValue::error_type{36};
const auto EXCEL_ERROR_REF = xlw::CellValue::error_type{23};
const auto EXCEL_ERROR_VALUE = xlw::CellValue::error_type{15};

enum CellValueType { CVT_NUMBER, CVT_BOOLEAN, CVT_STRING, CVT_ERROR, CVT_EMPTY, CVT_UNKNOWN };

template <typename T> CellValueType GetType(const T& item) {
    if (item.IsANumber())
        return CVT_NUMBER;
    else if (item.IsString())
        return CVT_STRING;
    else if (item.IsBoolean())
        return CVT_BOOLEAN;
    else if (item.IsError())
        return CVT_ERROR;
    else if (item.IsEmpty())
        return CVT_EMPTY;
    else
        return CVT_UNKNOWN;
}

bool operator<(const QFCellValue& lhs, const QFCellValue& rhs) {
    CellValueType lType = GetType(lhs);
    CellValueType rType = GetType(rhs);

    if (lType < rType)
        return true;
    else if (rType < lType)
        return false;
    else {
        if (lType == CVT_NUMBER)
            return lhs.NumericValue() < rhs.NumericValue();
        else if (lType == CVT_STRING)
            return lhs.StringValue() < rhs.StringValue();
        else if (lType == CVT_BOOLEAN)
            return lhs.BooleanValue() < rhs.BooleanValue();
        else if (lType == CVT_ERROR)
            return lhs.ErrorValue() < rhs.ErrorValue();
        else if (lType == CVT_EMPTY)
            return false;
        else if (lType == CVT_UNKNOWN) // for blank cells ???
            return false;
        else
            return true;
    }
}

template <typename SourceType> QFCellValue Convert2QFCellValue(const SourceType& item) {

    if (item.IsANumber())
        return QFCellValue(double(item));
    else if (item.IsString())
        return QFCellValue(std::string(item));
    else if (item.IsBoolean())
        return QFCellValue(bool(item));
    else if (item.IsError())
        return QFCellValue(item.ErrorValue(), true);
    else if (item.IsEmpty())
        return QFCellValue();
    else
        throw("Unknown type");
}

template <typename SourceType> bool CheckCellValue(const SourceType& item, double value) {
    if (item.IsANumber())
        return (double(item) == value);
    else
        return false;
}

template <typename SourceType> bool CheckCellValue(const SourceType& item, bool value) {
    if (item.IsBoolean())
        return (bool(item) == value);
    else
        return false;
}

template <typename SourceType> bool CheckCellValue(const SourceType& item, std::string value) {
    if (item.IsString())
        return (std::string(item) == value);
    else
        return false;
}

void Convert2CellValue(const QFCellValue& item, xlw::CellValue& result) {

    if (item.IsANumber())
        result = double(item);
    else if (item.IsString())
        result = std::string(item);
    else if (item.IsBoolean())
        result = bool(item);
    else if (item.IsError())
        result = xlw::CellValue::error_type{item.ErrorValue()};
    else if (item.IsEmpty())
        result.clear();
    else
        throw("Unknown type");
}
