#pragma once

#include "search_filter_model.h"
#include "search_result_model.h"

class DemoSearchSeed final {
public:
    static QList<SearchResultItem> results();
    static QList<SearchFilterItem> filters();
};
