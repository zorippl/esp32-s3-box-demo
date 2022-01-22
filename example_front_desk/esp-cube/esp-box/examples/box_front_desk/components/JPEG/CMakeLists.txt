idf_component_register(
    SRCS
        libjpeg.c
        jpeg-9a/jaricom.c
        jpeg-9a/jcomapi.c
        jpeg-9a/jutils.c
        jpeg-9a/jerror.c
        jpeg-9a/jmemmgr.c
        jpeg-9a/jdapimin.c
        jpeg-9a/jdapistd.c
        jpeg-9a/jdarith.c
        jpeg-9a/jdmaster.c
        jpeg-9a/jdinput.c
        jpeg-9a/jdmarker.c
        jpeg-9a/jdhuff.c
        jpeg-9a/jdmainct.c
        jpeg-9a/jdcoefct.c
        jpeg-9a/jddctmgr.c
        jpeg-9a/jdpostct.c
        jpeg-9a/jdsample.c
        jpeg-9a/jdcolor.c
        jpeg-9a/jquant2.c
        jpeg-9a/jquant1.c
        jpeg-9a/jdmerge.c
        jpeg-9a/jmemnobs.c
        jpeg-9a/jidctint.c
        jpeg-9a/jidctflt.c
        jpeg-9a/jidctfst.c
        jpeg-9a/jcparam.c
        jpeg-9a/jcapistd.c
        jpeg-9a/jcapimin.c
        jpeg-9a/jcinit.c
        jpeg-9a/jcmaster.c
        jpeg-9a/jccolor.c
        jpeg-9a/jchuff.c
        jpeg-9a/jcmarker.c
        jpeg-9a/jcmainct.c
        jpeg-9a/jccoefct.c
        jpeg-9a/jcarith.c
        jpeg-9a/jcdctmgr.c
        jpeg-9a/jcprepct.c
        jpeg-9a/jcsample.c
        jpeg-9a/jfdctint.c
        jpeg-9a/jfdctflt.c
        jpeg-9a/jfdctfst.c
        jpeg-9a/jdatasrc.c
        jpeg-9a/jdatadst.c
    INCLUDE_DIRS "include" ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -w)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DENCODE_LITTLE_ENDIAN")
