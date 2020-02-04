{
  "targets": [
    {
      "target_name": "picoscope-js",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ 
        "src/index.cc" ,
        "src/PicoScope/ps_wrap.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "C:\Program Files\Pico Technology\SDK\inc",

      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      'libraries': [
        "C:\Program Files\Pico Technology\SDK\lib\ps2000",
        "C:\Program Files\Pico Technology\SDK\lib\ps2000a"
      ]
    }
  ]
}