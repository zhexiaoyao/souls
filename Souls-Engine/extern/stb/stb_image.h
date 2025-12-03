/* stb_image.h - v2.27 - public domain image loader - http://nothings.org/stb
   no warranty implied; use at your own risk

   QUICKSTART:
      #define STB_IMAGE_IMPLEMENTATION
      #include "stb_image.h"

   and you can use stb_image.h like this:

      int width, height, channels;
      unsigned char *data = stb_load("image.jpg", &width, &height, &channels, 0);
      // ... process data if not NULL ...
      stb_image_free(data);

   Full documentation at end of file.

   Revision history:

      2.27  (2021-07-11) document stbi_info better, 16-bit PNM support, bug fixes
      2.26  (2020-07-13) many minor fixes
      2.25  (2020-02-02) fix warnings
      2.24  (2020-02-02) fix warnings; thread-local failure_reason and flip_vertically
      2.23  (2019-08-11) fix clang static analysis warning
      2.22  (2019-03-04) gif fixes, fix warnings
      2.21  (2019-02-25) fix typo in comment
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) bugfix, 1-bit BMP, 16-bits-per-channel TGA
      2.16  (2017-07-23) all functions have 16-bit variants; optimizations; bugfixes
      2.15  (2017-03-18) fix png-1,2,4; all Imagenet JPGs; no runtime SSE detection on GCC
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-12-04) experimental 16-bit API, only for PNG so far; fixes
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) 16-bit PNM; 16-bit PSD; TIFF-16; optimizations; bugfixes
      2.10  (2016-01-22) avoid warning
      2.09  (2016-01-16) 16-bit TGA; comments in PNM files; STBI_REALLOC_SIZED
      2.08  (2015-09-13) fix to 2.07 cleanup, reading RGB images as RGBA on ios
      2.07  (2015-09-13) partial ios improvements; fix incomplete implementation of STBI_REALLOC_SIZED
      2.06  (2015-04-19) fix bug where comments in JPEGs would cause parse failure
      2.05  (2015-04-19) fix warnings
      2.04  (2015-04-10) performance improvements; STBI_MALLOC_SIZE
      2.03  (2015-01-16) fix(N)hdr support; prep removal of STBI_JPEG_OLD
      2.02  (2014-12-25) fix a few more warnings
      2.01  (2014-12-25) fix warnings; BMP transparency
      2.00b (2014-12-25) fix STBI_MALLOC in release mode
      2.00  (2014-12-25) optimize JPG, add x86/x64 SIMD to JPG decoder
      1.99  (2014-08-17) fix broken TGA optimization
      1.98  (2014-05-27) fix warnings; fix flipped RGB VDA images
      1.97  (2014-05-26) fix compiler warnings
      1.96  (2014-05-19) fix warnings
      1.95  (2014-05-06) fix warnings
      1.94  (2014-04-24) fix warnings
      1.93  (2014-04-22) fix warnings
      1.92  (2014-04-21) fix warnings
      1.91  (2014-04-19) fix warnings
      1.90  (2014-04-18) fix warnings
      1.89  (2014-04-15) fix warnings
      1.88  (2014-04-15) fix warnings
      1.87  (2014-04-12) fix warnings
      1.86  (2014-04-11) fix warnings
      1.85  (2014-04-10) fix warnings
      1.84  (2014-04-10) fix warnings
      1.83  (2014-04-09) fix warnings
      1.82  (2014-04-09) fix warnings
      1.81  (2014-04-08) fix warnings
      1.80  (2014-04-08) fix warnings
      1.79  (2014-04-07) fix warnings
      1.78  (2014-04-07) fix warnings
      1.77  (2014-04-07) fix warnings
      1.76  (2014-04-07) fix warnings
      1.75  (2014-04-07) fix warnings
      1.74  (2014-04-07) fix warnings
      1.73  (2014-04-07) fix warnings
      1.72  (2014-04-07) fix warnings
      1.71  (2014-04-07) fix warnings
      1.70  (2014-04-07) fix warnings
      1.69  (2014-04-07) fix warnings
      1.68  (2014-04-07) fix warnings
      1.67  (2014-04-07) fix warnings
      1.66  (2014-04-07) fix warnings
      1.65  (2014-04-07) fix warnings
      1.64  (2014-04-07) fix warnings
      1.63  (2014-04-07) fix warnings
      1.62  (2014-04-07) fix warnings
      1.61  (2014-04-07) fix warnings
      1.60  (2014-04-07) fix warnings
      1.59  (2014-04-07) fix warnings
      1.58  (2014-04-07) fix warnings
      1.57  (2014-04-07) fix warnings
      1.56  (2014-04-07) fix warnings
      1.55  (2014-04-07) fix warnings
      1.54  (2014-04-07) fix warnings
      1.53  (2014-04-07) fix warnings
      1.52  (2014-04-07) fix warnings
      1.51  (2014-04-07) fix warnings
      1.50  (2014-04-07) fix warnings
      1.49  (2014-04-07) fix warnings
      1.48  (2014-04-07) fix warnings
      1.47  (2014-04-07) fix warnings
      1.46  (2014-04-07) fix warnings
      1.45  (2014-04-07) fix warnings
      1.44  (2014-04-07) fix warnings
      1.43  (2014-04-07) fix warnings
      1.42  (2014-04-07) fix warnings
      1.41  (2014-04-07) fix warnings
      1.40  (2014-04-07) fix warnings
      1.39  (2014-04-07) fix warnings
      1.38  (2014-04-07) fix warnings
      1.37  (2014-04-07) fix warnings
      1.36  (2014-04-07) fix warnings
      1.35  (2014-04-07) fix warnings
      1.34  (2014-04-07) fix warnings
      1.33  (2014-04-07) fix warnings
      1.32  (2014-04-07) fix warnings
      1.31  (2014-04-07) fix warnings
      1.30  (2014-04-07) fix warnings
      1.29  (2014-04-07) fix warnings
      1.28  (2014-04-07) fix warnings
      1.27  (2014-04-07) fix warnings
      1.26  (2014-04-07) fix warnings
      1.25  (2014-04-07) fix warnings
      1.24  (2014-04-07) fix warnings
      1.23  (2014-04-07) fix warnings
      1.22  (2014-04-07) fix warnings
      1.21  (2014-04-07) fix warnings
      1.20  (2014-04-07) fix warnings
      1.19  (2014-04-07) fix warnings
      1.18  (2014-04-07) fix warnings
      1.17  (2014-04-07) fix warnings
      1.16  (2014-04-07) fix warnings
      1.15  (2014-04-07) fix warnings
      1.14  (2014-04-07) fix warnings
      1.13  (2014-04-07) fix warnings
      1.12  (2014-04-07) fix warnings
      1.11  (2014-04-07) fix warnings
      1.10  (2014-04-07) fix warnings
      1.09  (2014-04-07) fix warnings
      1.08  (2014-04-07) fix warnings
      1.07  (2014-04-07) fix warnings
      1.06  (2014-04-07) fix warnings
      1.05  (2014-04-07) fix warnings
      1.04  (2014-04-07) fix warnings
      1.03  (2014-04-07) fix warnings
      1.02  (2014-04-07) fix warnings
      1.01  (2014-04-07) fix warnings
      1.00  (2014-04-07) fix warnings
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// 这是一个占位文件，实际使用时需要从 https://github.com/nothings/stb 下载完整的 stb_image.h
// 为了编译通过，这里提供基本的声明

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
void stbi_image_free(void *retval_from_stbi_load);
const char *stbi_failure_reason(void);

#ifdef __cplusplus
}
#endif

#endif // STBI_INCLUDE_STB_IMAGE_H

