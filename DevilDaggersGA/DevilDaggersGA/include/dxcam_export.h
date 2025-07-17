
#ifndef DXCAM_EXPORT_H
#define DXCAM_EXPORT_H

#ifdef DXCAM_STATIC_DEFINE
#  define DXCAM_EXPORT
#  define DXCAM_NO_EXPORT
#else
#  ifndef DXCAM_EXPORT
#    ifdef DXCam_EXPORTS
        /* We are building this library */
#      define DXCAM_EXPORT 
#    else
        /* We are using this library */
#      define DXCAM_EXPORT 
#    endif
#  endif

#  ifndef DXCAM_NO_EXPORT
#    define DXCAM_NO_EXPORT 
#  endif
#endif

#ifndef DXCAM_DEPRECATED
#  define DXCAM_DEPRECATED __declspec(deprecated)
#endif

#ifndef DXCAM_DEPRECATED_EXPORT
#  define DXCAM_DEPRECATED_EXPORT DXCAM_EXPORT DXCAM_DEPRECATED
#endif

#ifndef DXCAM_DEPRECATED_NO_EXPORT
#  define DXCAM_DEPRECATED_NO_EXPORT DXCAM_NO_EXPORT DXCAM_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef DXCAM_NO_DEPRECATED
#    define DXCAM_NO_DEPRECATED
#  endif
#endif

#endif /* DXCAM_EXPORT_H */
