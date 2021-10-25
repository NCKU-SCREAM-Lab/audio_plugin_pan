#pragma once

#define HRIR_AZIMUTH_NUM 25
#define HRIR_ELEVATION_NUM 50
#define HRIR_COEF_NUM 200

namespace hrir
{

extern float azi_angle[HRIR_AZIMUTH_NUM];
extern float ele_angle[HRIR_ELEVATION_NUM];
extern float coef_l[HRIR_AZIMUTH_NUM][HRIR_ELEVATION_NUM][HRIR_COEF_NUM];
extern float coef_r[HRIR_AZIMUTH_NUM][HRIR_ELEVATION_NUM][HRIR_COEF_NUM];

}
