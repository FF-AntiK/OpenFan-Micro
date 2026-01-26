import datetime

def write_version_to_file(filepath, contents):
    with open(filepath, "w", encoding="utf-8") as f:
        f.write(contents)

def make_version_string():
    now = datetime.datetime.now()
    year = now.strftime('%Y')
    month = now.strftime('%m')
    day = now.strftime('%d')

    version_header = \
f"""#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR   {year}
#define VERSION_MINOR   {month}
#define VERSION_PATCH   {day}

#endif /* VERSION_H */
"""

    print(f"New version {year}-{month}-{day}")
    return version_header

def make_version():
    print("--- Cutting version ---")
    version = make_version_string()
    write_version_to_file('src/version.h', version)

Import("env")
make_version()
