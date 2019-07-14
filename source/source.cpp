#include <algorithm>
#include <set>
#include <unordered_set>

#include "cellitem.h"
#include "cppinterface.h"

std::string QFAbout() {
    std::string result = std::string("Written by Duc Truong. \n") + "Using XLW " + XLW_VERSION + "\nCompiled with " + XLW_VERSION;
    return result;
}

/*************************************
utility functions & classes
*************************************/

struct LessThanCellValue {
    bool operator()(const QFCellValue& lhs, const QFCellValue& rhs) const { return lhs < rhs; }
};

struct LessThanCellValueVector {
    bool operator()(const std::vector<QFCellValue>& lhs, const std::vector<QFCellValue>& rhs) const {
        size_t minSize = std::min(lhs.size(), rhs.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (lhs[i] < rhs[i]) {
                return true;
            } else if (rhs[i] < lhs[i]) {
                return false;
            }
        }
        return (lhs.size() < rhs.size());
    }
};

template <class SetType> struct SetInsert {
    void operator()(SetType& value, typename SetType::value_type x) { value.insert(x); }
};

template <class VectorType> struct VectorInsert {
    void operator()(VectorType& value, typename VectorType::value_type x) { value.push_back(x); }
};

template <class ContainerType, class InsertClass> ContainerType CellMatrix2Container(const CellMatrix& x, InsertClass insertFunction) {
    ContainerType items;

    for (size_t row = 0; row < x.RowsInStructure(); ++row) {
        for (size_t col = 0; col < x.ColumnsInStructure(); ++col) {
            insertFunction(items, Convert2QFCellValue(x(row, col)));
        }
    }
    return items;
}

template <class ContainerType> CellMatrix Container2CellMatrix(const ContainerType& items) {
    CellMatrix result{items.size(), 1};

    size_t i{0};
    for (auto& item : items) {
        Convert2CellValue(item, result(i, 0));
        ++i;
    }
    return result;
}

std::vector<const CellMatrix*> Arguments2Vector() { return std::vector<const CellMatrix*>(); }

template <typename... Types> std::vector<const CellMatrix*> Arguments2Vector(const CellMatrix* first, Types... rest) {
    std::vector<const CellMatrix*> result = Arguments2Vector(rest...);
    if ((first->RowsInStructure() * first->ColumnsInStructure()) != 0.0) {
        result.insert(begin(result), first);
    }
    return result;
}

/*************************************
sorts, unique, union, intersection functions
*************************************/

template <class SetClass> CellMatrix SortSet(const CellMatrix& x) {
    return Container2CellMatrix(CellMatrix2Container<SetClass>(x, SetInsert<SetClass>()));
}

CellMatrix QFSort(const CellMatrix& x) { return SortSet<std::multiset<QFCellValue, LessThanCellValue>>(x); }

CellMatrix QFUnique(const CellMatrix& x) {
    // could use unordered_set here, but have to write hash function
    return SortSet<std::set<QFCellValue, LessThanCellValue>>(x);
}

CellMatrix Union(const std::vector<const CellMatrix*>& cells) {
    using SetContainer = std::set<QFCellValue, LessThanCellValue>;
    SetContainer result;
    for (auto& cell : cells) {
        SetContainer item = CellMatrix2Container<SetContainer>(*cell, SetInsert<SetContainer>());
        result.insert(begin(item), end(item));
    }
    return Container2CellMatrix(result);
}

CellMatrix Intersection(const std::vector<const CellMatrix*>& cells) {
    using SetContainer = std::set<QFCellValue, LessThanCellValue>;
    SetContainer result{CellMatrix2Container<SetContainer>(*cells[0], SetInsert<SetContainer>())};

    for (auto iter = cbegin(cells) + 1; iter != cend(cells); ++iter) {
        auto items = CellMatrix2Container<SetContainer>(**iter, SetInsert<SetContainer>());
        SetContainer temp;
        std::set_intersection(cbegin(items), cend(items), cbegin(result), cend(result),
                              std::insert_iterator<SetContainer>(temp, begin(temp)), LessThanCellValue());
        result = std::move(temp);
    }

    return Container2CellMatrix(result);
}

CellMatrix                           // get the intersection items of tables
QFIntersection(const CellMatrix& x1, // 1st item
               const CellMatrix& x2, // 2nd item
               const CellMatrix& x3, // 3rd item
               const CellMatrix& x4, // 4th item
               const CellMatrix& x5, // 5th item
               const CellMatrix& x6) // 6th item
{
    auto result = Arguments2Vector(&x1, &x2, &x3, &x4, &x5, &x6);
    return Intersection(result);
}

CellMatrix                    // get the intersection items of tables
QFUnion(const CellMatrix& x1, // 1st item
        const CellMatrix& x2, // 2nd item
        const CellMatrix& x3, // 3rd item
        const CellMatrix& x4, // 4th item
        const CellMatrix& x5, // 5th item
        const CellMatrix& x6) // 6th item
{
    auto result = Arguments2Vector(&x1, &x2, &x3, &x4, &x5, &x6);
    return Union(result);
}

/*************************************
exact match, lookup functions
*************************************/

CellMatrix QFExactVLookup(const CellMatrix& lookup, const CellMatrix& table, const CellMatrix& outputTable) {
    if (lookup.ColumnsInStructure() != table.ColumnsInStructure()) {
        throw("Lookup items and table do not have the same number of fields.");
    }

    if (outputTable.RowsInStructure() != table.RowsInStructure()) {
        throw("Table and output table do not have the same number of records.");
    }

    // construct the map
    std::map<std::vector<QFCellValue>, size_t, LessThanCellValueVector> tableItems;

    for (size_t row = 0; row < table.RowsInStructure(); ++row) {
        std::vector<QFCellValue> rowItem;
        for (size_t col = 0; col < table.ColumnsInStructure(); ++col) {
            rowItem.emplace_back(Convert2QFCellValue(table(row, col)));
        }
        tableItems[rowItem] = row;
    }

    CellMatrix result{lookup.RowsInStructure(), outputTable.ColumnsInStructure()};

    for (size_t row = 0; row < lookup.RowsInStructure(); ++row) {
        std::vector<QFCellValue> rowItem;
        for (size_t col = 0; col < lookup.ColumnsInStructure(); ++col) {
            rowItem.emplace_back(Convert2QFCellValue(lookup(row, col)));
        }

        if (auto iter = tableItems.find(rowItem); iter != end(tableItems)) {
            double index = double(iter->second);
            for (size_t col = 0; col < outputTable.ColumnsInStructure(); ++col) {
                result(row, col) = outputTable(index, col);
            }
        } else {
            for (size_t col = 0; col < outputTable.ColumnsInStructure(); ++col) {
                result(row, col) = EXCEL_ERROR_NA;
            }
        }
    }
    return result;
}

// reuse ExactVLookup to reduce code duplication
CellMatrix QFExactMatch(const CellMatrix& lookup, const CellMatrix& table, const CellMatrix& oneIndex) {
    // get the base index from oneIndex
    size_t baseIndex{0};
    if (oneIndex.ColumnsInStructure() * oneIndex.RowsInStructure() != 0) {
        if (!(CheckCellValue(oneIndex(0, 0), 0.0) && CheckCellValue(oneIndex(0, 0), false))) {
            baseIndex = 1;
        }
    }
    CellMatrix indices(table.RowsInStructure(), 1);

    for (size_t row = 0; row < table.RowsInStructure(); ++row) {
        indices(row, 0) = int(row + baseIndex);
    }
    return QFExactVLookup(lookup, table, indices);
}

CellMatrix QFFilter(const CellMatrix& lookup, const CellMatrix& table, const CellMatrix& outputTable, const CellMatrix& includeLookup) {
    if (lookup.ColumnsInStructure() != table.ColumnsInStructure()) {
        throw("Lookup items and table do not have the same number of fields.");
    }

    if (outputTable.RowsInStructure() != table.RowsInStructure()) {
        throw("Table and output table do not have the same number of records.");
    }

    size_t lookupIncluded{0};
    if (includeLookup.ColumnsInStructure() * includeLookup.RowsInStructure() != 0) {
        if (!(CheckCellValue(includeLookup(0, 0), 0.0) && CheckCellValue(includeLookup(0, 0), false))) {
            lookupIncluded = 1;
        }
    }
    // construct the map
    std::map<std::vector<QFCellValue>, std::vector<size_t>, LessThanCellValueVector> tableItems;

    for (size_t row = 0; row < table.RowsInStructure(); ++row) {
        std::vector<QFCellValue> rowItem;
        for (size_t col = 0; col < table.ColumnsInStructure(); ++col) {
            rowItem.emplace_back(Convert2QFCellValue(table(row, col)));
        }
        tableItems[rowItem].push_back(row);
    }

    std::vector<decltype(tableItems.begin())> resultIters;
    size_t totalRows{0};

    for (size_t row = 0; row < lookup.RowsInStructure(); ++row) {
        std::vector<QFCellValue> rowItem;
        for (size_t col = 0; col < lookup.ColumnsInStructure(); ++col) {
            rowItem.emplace_back(Convert2QFCellValue(lookup(row, col)));
        }

        if (auto iter = tableItems.find(rowItem); iter != end(tableItems)) {
            totalRows += iter->second.size();
            resultIters.push_back(iter);
        }
    }

    auto colOffset = lookupIncluded * lookup.ColumnsInStructure();
    CellMatrix result{totalRows, outputTable.ColumnsInStructure() + colOffset};

    size_t row{0};
    for (auto&& iter : resultIters) {
        for (auto&& iOutput : iter->second) {
            if (colOffset) {
                for (size_t col = 0; col < colOffset; ++col) {
                    result(row, col) = iter->first[col];
                }
            }
            for (size_t col = 0; col < outputTable.ColumnsInStructure(); ++col) {
                result(row, col + colOffset) = outputTable(iOutput, col);
            }
        }
    }

    return result;
}

/*************************************
Pivot table functions
*************************************/

template <class OutputClass, class InitializationClass, class IncrementClass>
CellMatrix PivotTable(const CellMatrix& value, const CellMatrix& horizontal, const CellMatrix& vertical, InitializationClass initFunctor,
                      IncrementClass incrementFunctor) {
    bool hasVertical = (vertical.RowsInStructure() * vertical.ColumnsInStructure()) > 0;

    if ((horizontal.RowsInStructure() != value.RowsInStructure()) &&
        ((!hasVertical) || (horizontal.RowsInStructure() != vertical.RowsInStructure()))) {
        throw("The inputs must have the same number of records.");
    }
    // construct the map
    std::map<std::vector<QFCellValue>, std::map<std::vector<QFCellValue>, OutputClass, LessThanCellValueVector>, LessThanCellValueVector>
        tableItems;
    std::set<std::vector<QFCellValue>, LessThanCellValueVector> columnSet;
    std::vector<QFCellValue> emptyItem;

    for (size_t row = 0; row < horizontal.RowsInStructure(); ++row) {
        std::vector<QFCellValue> horizontalItem, verticalItem, valueItem;

        for (size_t col = 0; col < value.ColumnsInStructure(); ++col) {
            valueItem.emplace_back(Convert2QFCellValue(value(row, col)));
        }
        // horizontal and the total
        for (size_t col = 0; col < horizontal.ColumnsInStructure(); ++col) {
            horizontalItem.emplace_back(Convert2QFCellValue(horizontal(row, col)));
        }

        if (auto hIter = tableItems.find(horizontalItem); hIter == end(tableItems)) {
            tableItems[horizontalItem][emptyItem] = initFunctor(valueItem);
        } else {
            (hIter->second)[emptyItem] = incrementFunctor(valueItem, (hIter->second)[emptyItem]);
        }
        // vertical and the sub item
        if (hasVertical) {
            for (size_t col = 0; col < vertical.ColumnsInStructure(); ++col) {
                verticalItem.emplace_back(Convert2QFCellValue(vertical(row, col)));
            }

            if (auto vIter = tableItems[horizontalItem].find(verticalItem); vIter == end(tableItems[horizontalItem])) {
                tableItems[horizontalItem][verticalItem] = initFunctor(valueItem);
            } else {
                vIter->second = incrementFunctor(valueItem, vIter->second);
            }
            columnSet.insert(verticalItem);
        }
    }

    CellMatrix result{tableItems.size() + vertical.ColumnsInStructure(), horizontal.ColumnsInStructure() + columnSet.size() + 1};

    // set up headers
    size_t col = horizontal.ColumnsInStructure();
    for (auto& column : columnSet) {
        for (size_t i = 0; i < column.size(); ++i) {
            Convert2CellValue(column[i], result(i, col));
        }
        ++col;
    }

    // write items & their values
    size_t row = vertical.ColumnsInStructure();

    for (auto& rowItem : tableItems) {
        // write the row headers
        for (col = 0; col < horizontal.ColumnsInStructure(); ++col) {
            Convert2CellValue(rowItem.first[col], result(row, col));
        }
        // write the values
        for (auto& column : columnSet) {
            if (auto iter = rowItem.second.find(column); iter != end(rowItem.second)) {
                Convert2CellValue(iter->second, result(row, col));
            }
            ++col;
        }

        // write the aggregate
        Convert2CellValue(rowItem.second[emptyItem], result(row, col));
        ++row;
    }

    return result;
}

struct CountInitialization {
    double operator()(std::vector<QFCellValue>& value) { return 1.0; }
};

struct CountIncrement {
    double operator()(std::vector<QFCellValue>& value, double current) { return (current + 1); }
};

CellMatrix QFPivotCount(const CellMatrix& horizontal, const CellMatrix& vertical) {
    CellMatrix value{horizontal.RowsInStructure(), 1};

    return PivotTable<double>(value, horizontal, vertical, CountInitialization(), CountIncrement());
}

struct SumInitialization {
    double operator()(const std::vector<QFCellValue>& value) { return double(value[0]); }
};

struct SumIncrement {
    double operator()(const std::vector<QFCellValue>& value, double current) { return current + double(value[0]); }
};

CellMatrix QFPivotSum(const CellMatrix& value, const CellMatrix& horizontal, const CellMatrix& vertical) {
    return PivotTable<double>(value, horizontal, vertical, SumInitialization(), SumIncrement());
}

struct MaxIncrement {
    double operator()(const std::vector<QFCellValue>& value, double current) { return std::max(current, double(value[0])); }
};

struct MinIncrement {
    double operator()(const std::vector<QFCellValue>& value, double current) { return std::min(current, double(value[0])); }
};

CellMatrix QFPivotMax(const CellMatrix& value, const CellMatrix& horizontal, const CellMatrix& vertical) {
    return PivotTable<double>(value, horizontal, vertical, SumInitialization(), MaxIncrement());
}

CellMatrix QFPivotMin(const CellMatrix& value, const CellMatrix& horizontal, const CellMatrix& vertical) {
    return PivotTable<double>(value, horizontal, vertical, SumInitialization(), MinIncrement());
}
