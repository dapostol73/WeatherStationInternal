#ifndef _CANADIAN_HYDROGRAPHIC_INFO_H_
#define _CANADIAN_HYDROGRAPHIC_INFO_H_

// Canadian Hydrographic Service API information
// https://tides.gc.ca/en/web-services-offered-canadian-hydrographic-service
// https://api-sine.dfo-mpo.gc.ca/swagger-ui/index.html

struct CanadianHyrdographicInfo
{
    const char *Host = "api-sine.dfo-mpo.gc.ca"; //IP address of the Canadian Hydrographic Service
    const char *StationID = ""; // ID of the station to get data for (not Station Code/Number)
    const char *TimeSeriesCode = "wlp-hilo"; // Time series code for water level predictions (High/Low)

    CanadianHyrdographicInfo() = default;

    CanadianHyrdographicInfo(const char* stationID)
    {
        StationID = stationID;
    }
};

#endif