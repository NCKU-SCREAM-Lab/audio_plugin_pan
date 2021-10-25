import os
import sys
import scipy.io

def main():
    if len(sys.argv) != 3+1:
        print("input files: [hrir] [header] [cpp]")
        sys.exit(1)

    hrir_file = sys.argv[1]
    header_file = sys.argv[2]
    cpp_file = sys.argv[3]
    print("input files:", hrir_file)
    print("header files:", header_file)
    print("cpp files:", cpp_file)

    num_azimuth = 25
    num_elevation = 50
    num_coefficient = 200

    out_file = open(header_file, 'w')
    out_file.write("#pragma once\n"
                   "\n")
    out_file.write(f"#define HRIR_AZIMUTH_NUM {num_azimuth}\n"
                   f"#define HRIR_ELEVATION_NUM {num_elevation}\n"
                   f"#define HRIR_COEF_NUM {num_coefficient}\n"
                   "\n")
    out_file.write("namespace hrir\n"
                   "{\n"
                   "\n"
                   "extern float azi_angle[HRIR_AZIMUTH_NUM];\n"
                   "extern float ele_angle[HRIR_ELEVATION_NUM];\n"
                   f"extern float coef_l[HRIR_AZIMUTH_NUM][HRIR_ELEVATION_NUM][HRIR_COEF_NUM];\n"
                   f"extern float coef_r[HRIR_AZIMUTH_NUM][HRIR_ELEVATION_NUM][HRIR_COEF_NUM];\n"
                   "\n"
                   "}\n")
    out_file.close()

    #write_csv("l_hrtf_csv", l_hrtf_file, out_file)
    #write_csv("r_hrtf_csv", l_hrtf_file, out_file)

    out_file = open(cpp_file, 'w')
    out_file.write('#include "hrir.h"\n'
                   "\n"
                   "namespace hrir\n"
                   "{\n"
                   "\n")

    azimuth = [-80, -65, -55] + list(range(-45, 50 ,5)) + [55, 65, 80]
    elevation = [-45 + 5.625*i for i in range(50)]
    out_file.write("float azi_angle[HRIR_AZIMUTH_NUM] = {" + ", ".join(map(str, azimuth)) + "};\n"
                   "float ele_angle[HRIR_ELEVATION_NUM] = {" + ", ".join(map(str, elevation)) + "};\n"
                   "\n")

    hrir_mat = scipy.io.loadmat(hrir_file)
    for key, var in [('hrir_l', 'coef_l'), ('hrir_r', 'coef_r')]:
        out_file.write(f"float {var}[HRIR_AZIMUTH_NUM][HRIR_ELEVATION_NUM][HRIR_COEF_NUM] = " "{\n")
        indent = 4;
        for i in range(num_azimuth):
            out_file.write(" "*indent + "{\n")
            indent += 4
            for j in range(num_elevation):
                out_file.write(" "*indent + "{")
                #for k in range(num_coefficient):
                out_file.write(", ".join(map(str, hrir_mat[key][i,j])))
                out_file.write("},\n")
            indent -= 4
            out_file.write(" "*indent + "},\n")
        out_file.write("};\n")

    out_file.write("\n"
                   "}\n")
    out_file.close()


def write_csv(var_name, csv_file, out_file):
    out_file.write(f'const char *{var_name} =')
    with open(csv_file) as f:
        for line in f.readlines():
            line = line.strip()
            out_file.write('\n    "' + line + '\\n"')
    out_file.write(';\n')

if __name__ == '__main__':
    main()
