#pragma once

#include <xlw/MyContainers.h>
#include <xlw/CellMatrix.h>
#include <xlw/DoubleOrNothing.h>
#include <xlw/ArgList.h>

using namespace xlw;
std::string
QFAbout();

CellMatrix // sort a table
QFSort(const CellMatrix& x);

CellMatrix // get the unique items of a table
QFUnique(const CellMatrix& x);

CellMatrix // get the intersection items of tables
QFIntersection(const  CellMatrix& x1, // 1st item
               const  CellMatrix& x2, // 2nd item
               const  CellMatrix& x3, // 3rd item
               const  CellMatrix& x4, // 4th item
               const  CellMatrix& x5, // 5th item
               const  CellMatrix& x6); // 6th item

CellMatrix // get the union items of tables
QFUnion(const CellMatrix& x1, // 1st item
        const CellMatrix& x2, // 2nd item
        const CellMatrix& x3, // 3rd item
        const CellMatrix& x4, // 4th item
        const CellMatrix& x5, // 5th item
        const CellMatrix& x6); // 6th item

CellMatrix // match function
QFExactMatch(const CellMatrix& lookup,   // items to look up
             const CellMatrix& table,    // table to be looked up against
             const CellMatrix& oneIndex); // 0, false or empty to have 0-based index, otherwise 1-based index

CellMatrix // VLookup function
QFExactVLookup(const CellMatrix& lookup, // items to look up
                const CellMatrix& table, // table to be looked up against
                const CellMatrix& outputTable); // table values to be printed out

CellMatrix // multi lookup function
QFFilter(const CellMatrix& lookup, // items to look up
    const CellMatrix& table, // table to be looked up against
    const CellMatrix& outputTable,  // table values to be printed out
    const CellMatrix& includeLookup); // non zero/false value to output the lookup values

CellMatrix
QFPivotCount(const CellMatrix& horizontal,
            const CellMatrix& vertical);

CellMatrix
QFPivotSum(const CellMatrix& value,
           const CellMatrix& horizontal,
           const CellMatrix& vertical);

CellMatrix
QFPivotMax(const CellMatrix& value,
           const CellMatrix& horizontal,
           const CellMatrix& vertical);

CellMatrix
QFPivotMin(const CellMatrix& value,
           const CellMatrix& horizontal,
           const CellMatrix& vertical);

