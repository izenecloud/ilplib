Language processing libraries 
=============================================
*Encapsulation for multi-languages purpose.*

### Features
* _Language identifier_. The language identifier library is used to recognize languages, corresponding technical report could be accessed [here](https://github.com/izenecloud/ilplib/blob/master/docs/pdf/langid-tr.pdf).

* _Multi-language analyzerl_. This is an encapsulation for [icma](https://github.com/izenecloud/icma) and [ijma](https://github.com/izenecloud/ijma). Language identifier is also used to decide which analyzer is used. Corresponding technical report could be accessed [here](https://github.com/izenecloud/ilplib/blob/master/docs/pdf/la-comprehension-tr.pdf). 

* _Tokenizers_. We also delivered several utility tokenizers for Chinese verticals. For search engine purpose, vertical portals always have different requirements on tokenization.

### Dependencies
We've just switched to `C++ 11` for SF1R recently, and `GCC 4.8` is required to build SF1R correspondingly. We do not recommend to use Ubuntu for project building due to the nested references among lots of libraries. CentOS / Redhat / Gentoo / CoreOS are preferred platform. You also need `CMake` and `Boost 1.56` to build the repository . Here are the dependent repositories list:

* __[cmake](https://github.com/izenecloud/cmake)__: The cmake modules required to build all iZENECloud C++ projects.

* __[icma](https://github.com/izenecloud/icma)__: The Chinese morphological analyzer.

* __[ijma](https://github.com/izenecloud/ijma)__: The Japanese morphological analyzer.

### License
The project is published under the Apache License, Version 2.0:
http://www.apache.org/licenses/LICENSE-2.0
