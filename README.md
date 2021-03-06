# The SimSoft Library
The _**Sim**on Bolivar & Company **Soft**ware **Library**_ is (intended to be) a cross-platform multilingual library containing various useful features to ease software development, such as:

* Container structures (vectors, hashmaps, queues, trees, ...)
* String-related functions & classes (strings, string views, regular expressions)
* Random number generators
* Generalized IO streams (similar to SDL's *RWops* or C++ *std::iostream*; read, write, seek, etc. for files, memory, structs, strings, etc.)
* Runtime dynamic library (i.e. shared object) loading

The library's base implementation is written in C11 [to provide a fun challenge for everyone **:^)** ].

# Table of Contents
- [The SimSoft Library](#the-simsoft-library)
- [Table of Contents](#table-of-contents)
- [Project Goals](#project-goals)
- [Building](#building)
- [Documentation](#documentation)
- [Contributing](#contributing)
  - [Style Guide](#style-guide)
  - [Code of Conduct](#code-of-conduct)
    - [Our Pledge](#our-pledge)
    - [Our Standards](#our-standards)
    - [Our Responsibilities](#our-responsibilities)
    - [Scope](#scope)
    - [Enforcement](#enforcement)

# Project Goals
This library aims to be useful, customizable, heavy documented, and able to be integrated into a variety of programming languages without compromising their respective idioms/aesthetic (though C & C++ are currently the main priority).

Elaborating on the last point, we don't want the programmer to have to insert a bunch of ~~ugly~~ C code in the middle of an ~~ugly~~ C++ codebase. We'll take care of that for you; for example, the C++ variant of the library is planned to make use of the language's object oriented, RAII, references, operator overloading, and templating features.

For a more verbose and in-depth to-do list, see `TODO.txt` or visit our [Trello board](https://trello.com/b/ynDyCaWr/simsoft-library).

# Building
Install *gcc* (or *clang*) and *GNU make*, then type `make libsim` into a Bourne-compatible shell to build the library. Support for other compilers + Windows CMD.EXE will be added later on.

A unit test executable can be built by typing `make exesimtest` into the shell.

Various options exist to customize how the SimSoft library and associated build targets are constructed. Details on what options and other build targets are available can be found by typing `make help` into the shell.

# Documentation
Documentation is available through the use of Doxygen. Three files are available to be used to generate documentation:

- `c.doxyfile` - Generates C-style documentation for the C API.
- `cpp.doxyfile` - Generates C++-style documentation for the C and C++ APIs.
- `Doxyfile` - The default; contents are equivalent to `cpp.doxyfile`.

To generate documentation, run `doxygen` (or `doxygen <doxyfile>` on one of the files listed above).

# Contributing
Anybody that wishes to contribute to the development of the SimSoft library must follow the SimSoft style guide and the Code of Conduct.

## Style Guide
Comming soon...

## Code of Conduct
This library's contributor Code of Conduct is adapted from the [Contributor Covenant](https://www.contributor-covenant.org), version 1.4, available at https://www.contributor-covenant.org/version/1/4/code-of-conduct.html

For answers to common questions about this code of conduct, see
https://www.contributor-covenant.org/faq

### Our Pledge
In the interest of fostering an open and welcoming environment, we as contributors and maintainers pledge to making participation in our project and our community a harassment-free experience for everyone, regardless of age, body size, disability, ethnicity, sex characteristics, gender identity and expression, level of experience, education, socioeconomic status, nationality, personal appearance, race, religion, or sexual identity and orientation.

### Our Standards
Examples of behavior that contributes to creating a positive environment include:
* Using welcoming and inclusive language
* Being respectful of differing viewpoints and experiences
* Gracefully accepting constructive criticism
* Focusing on what is best for the community
* Showing empathy towards other community members

Examples of unacceptable behavior by participants include:
* The use of sexualized language or imagery and unwelcome sexual attention or advances
* Trolling, insulting/derogatory comments, and personal or political attacks
* Public or private harassment
* Publishing others' private information, such as a physical or electronic address, without explicit permission
* Other conduct which could reasonably be considered inappropriate in a professional setting

### Our Responsibilities
Project maintainers are responsible for clarifying the standards of acceptable behavior and are expected to take appropriate and fair corrective action in response to any instances of unacceptable behavior.

Project maintainers have the right and responsibility to remove, edit, or reject comments, commits, code, wiki edits, issues, and other contributions that are not aligned to this Code of Conduct, or to ban temporarily or permanently any contributor for other behaviors that they deem inappropriate, threatening, offensive, or harmful.

### Scope
This Code of Conduct applies both within project spaces and in public spaces when an individual is representing the project or its community. Examples of representing a project or community include using an official project e-mail address, posting via an official social media account, or acting as an appointed representative at an online or offline event. Representation of a project may be further defined and clarified by project maintainers.

### Enforcement
Instances of abusive, harassing, or otherwise unacceptable behavior may be reported by contacting the project team at [simsoft@simbol.co](mailto:simsoft@simbol.co). All complaints will be reviewed and investigated and will result in a response that is deemed necessary and appropriate to the circumstances. The project team is obligated to maintain confidentiality with regard to the reporter of an incident. Further details of specific enforcement policies may be posted separately.

Project maintainers who do not follow or enforce the Code of Conduct in good faith may face temporary or permanent repercussions as determined by other members of the project's leadership.
