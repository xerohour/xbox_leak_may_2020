Changes in CDImage v2.45 over 2.44
- Fixed problem where the ISO part of a UDF bridge disk reported disk size to be 
  one sector less than the UDF part

- if -uv (Video Zone) option is used, UDF version 1.02 is generated otherwise 1.50 is generated.

Changes in CDImage v2.44 over 2.43

- Changed short name generation algorithm to be O(n^2) from O(n^3)

- Added -yf switch for even faster shortname generation - files may get different shortnames on different images

Changes in CDIMAGE v2.43 over 2.42

- Fixed problem combining -u1 -b (El Torito with UDF), and allow -u2 with -b.

- Disallow two digit year with -t to prevent 00 from being interpreted as 1900.

- Fixed problem introduced with 2.42 when no longname switches (-n, -j, or -u)
  were used (8.3 names in primary namespace).

Changes in CDImage v2.42 over 2.40

- Enhanced UDF bridge options

- Added UDF Video Zone support

- More output better formated if redirected to file

- Fixed minor bugs

Changes in CDIMAGE V2.40 over 2.39:

- Enhanced UDF support
       -ys switch to store files as sparse (Windows 98 does not read these files)
       -ye switch to embed file information in directory information
       -yf swtich to embed file data in file information
       -o support added for UDF

- Sources files opened read only to prevent changes from happening during read

- Size for source files checked when opened, a warning is displayed if a difference is found
  which would mean the file has been modified between the time cdimage was started and
  this file was read

- Output changed to format better if redirected to file

- Added support to set the El Torito boot segment

Changes in CDIMAGE V2.39 over V2.38:

- Changes to DVD/UDF structures.

- Changed Microsoft's phone number to (425) area code in volume header.

- Open target image file with exclusive access to prevent another process
  from reading the image until it is completed.


Changes in CDIMAGE V2.38 over V2.37:

- Added support for El Torito "floppy emulation mode" bootable CD
  creation.  If the -b file is exactly 1228800, 1474560, or 2949120
  bytes, the file is assumed to be a floppy disk image and is encoded
  as El Torito "floppy emulation mode".  If the -b file is other than
  one of these sizes, the image is still encoded as El Torito "no
  emulation mode".


Changes in CDIMAGE V2.37 over V2.36:

- Rebuilt with msvcrt to make it smaller, no functional changes.


Changes in CDIMAGE V2.36 over V2.34:

- Added -js switch to allow custom (localized) "readme.txt" file to be
  placed in the root directory of the non-Joliet portion of a Joliet
  image created using -j2.

- Added -us switch similar to -js switch to allow custom "readme.txt"
  file for UDF images created with -u2.

- Added -yb switch to enable creation of 512-byte logical block sizes
  for testing purposes only.  DO NOT USE THIS OPTION TO CREATE DISCS
  TO BE RELEASED OUTSIDE MICROSOFT.  If you have more questions about
  this option, please contact PankajL or Tommcg.


Changes in CDIMAGE V2.34 over V2.33:

- Using -q switch with -o switch will now read all source files and
  report size of optimized image without actually creating an image.


Changes in CDIMAGE V2.33 over V2.32:

- Added switch (-k) to continue creating image even though failed to
  open some of the source files.  A warning will be displayed when
  this occurs, and the resulting image will not contain those files.

- Use less memory when creating image without -o switch.


Changes in CDIMAGE V2.32 over V2.12:

There have been many changes to CDIMAGE 2.32 since 2.12.  This is only
a partial listing.  Note that CDIMAGE 2.32 requires Windows NT 4.0 and
will not run on Windows NT 3.51 or Windows 95.

- Several fixes for very large directories and very large number of
  directories.

- Fixed problem with recording timestamps one hour off during daylight
  savings time.

- Fixed MSCDEX compatibility problem when directory entries happen to
  be aligned exactly with the end of a sector.

- Fixed an occasional "hang" while using the -o switch.

- Added switch to include hidden files and directories from source and
  make them hidden on the CD (-h).

- Added switch to scan source files to report warnings and determine
  image size without actually creating an image file (-q).

- Added support for encoding digital signatures on CDs (-s).  This
  requires a separate program (rpcsig) for issuing signatures and a
  new program (cdverify rather than crc) for verifying the contents.

- Added support for producing UDF (ISO 13346) file system structures
  for DVD media (-u1, -u2).  This is still in the experimental stage
  and is not recommended for general use.

- Duplicate file detection now employs MD5 hashing rather than CRC for
  determining file equivalence (more reliable).  Also, the -o option
  now defaults to MD5 hash testing rather than slower binary comparison
  which is still available (-oc).

- Added various testing switches (-y) for generating images to test
  CD-ROM file systems.  These are not for general use.

- Added and improved warnings and error messages.

- Various performance enhancements.


Changes in CDIMAGE V2.12 over V2.11:

- Added support for El Torito bootable CD creation (-b switch).  This
  allows operating system products to perform floppyless installation
  from CD-ROM media, provided the machine has an El Torito compliant
  BIOS to support booting from CD-ROM.  For more information about this
  option, contact tommcg.

- Added support for "AutoCRC" generation (-x switch).  AutoCRC encodes
  a CRC-32 signature in the image to cause the computed CRC value of
  the image or CD to always arrive at a pre-computed fixed value.  New
  versions of CRC.EXE will detect this CRC signature and simply tell
  you if the CD is "valid" or "corrupt".  Older versions of CRC.EXE
  will still work on CDs with AutoCRC -- they will simply return the
  value 0xFFFFFFFF if the image or CD is valid.  Enabling the AutoCRC
  option will add 4096 bytes to an image file.

- Added support for longer-than-256-character path name parsing when
  creating a Joliet CD with the -j1 or -j2 options.

- CDIMAGE will still not run on Windows 95 or Win32s since it requires
  operating system support for overlapped file i/o.  Previous versions
  of CDIMAGE would fail in various ways when trying to run on Win95 or
  Win32s.  CDIMAGE will now display an appropriate error message when
  trying to run on Win95 or Win32s.


Changes in CDIMAGE V2.11 over V2.10:

- Fixed bug that resulted in all files in root directory having corrupt
  contents.

- Fixed bug in generation of short names on Joliet CDs when not using
  the -o options.


Changes in CDIMAGE V2.10 over V2.06:

- Recording long filenames on CDs has caused much concern lately.  To
  help people create long filenames on CDs that are compatible with
  both Windows 95 and Windows NT 3.51, I've added a new switch -nt that
  allows you to encode long filenames up to the limits that won't cause
  problems on NT 3.51.  Using the -nt switch, you can safely create CDs
  that contain long filenames that are compatible with both Windows 95
  and Windows NT 3.51.  This is different than the Joliet Unicode file
  names -- Windows NT 3.51 does not recognize Joliet Unicode filenames.
  Using the -nt switch, here are the limits on long filenames:

      File names can contain up to 221 characters, but cannot
      have lowercase characters, spaces, or extended characters.

      Directory names can contain up to 37 characters, but cannot
      have lowercase characters, spaces, or extended characters.

- Joliet Unicode filenames are supported by Windows 95 and the next
  release of Windows NT (4.00).  The current shipping version of
  Windows NT (3.51) does not support Joliet Unicode filenames.  There
  are two new options in CDIMAGE for creating CDs with Joliet Unicode
  filenames: -j1 and -j2.  Using the -j1 switch will create a CD that
  contains both Joliet Unicode filenames and "generated" conventional
  ISO-9660 filenames that can be seen by operating systems that do not
  support Joliet Unicode filenames (such as DOS, Windows 3.x, NT 3.51).
  The generated ISO-9660 filenames will conform to DOS 8.3 and ISO
  naming restrictions.  Using the -j2 switch will create a CD that only
  contains Joliet Unicode filenames, so it can only be used with an
  operating system that supports Joliet Unicode filenames (ie, Windows
  95 and Windows NT 4.00).  An operating system that does not support
  Joliet Unicode filenames will only see a single file on a CD created
  with the -j2 option: a "readme.txt" that states the CD is only for
  use with operating systems that support Joliet Unicode filenames.
  With the -j1 and -j2 switches, here are the limits on long filenames:

      File and directory names can contain up to 110 Unicode
      characters, including spaces, lowercase characters, and
      any other Unicode characters other than control characters
      (such as asterisks, colons, question marks, etc.).

- In previous versions of CDIMAGE, timestamps were encoded as "GMT"
  time.  In V2.10, time stamps are encoded by default as "local" time
  with an appropriate "offset from GMT" value in the timestamp.  To
  override the new default behavior, there is a new -g switch that
  encodes time stamps as "GMT".  There are some behaviorial differences
  in the way Windows 95, Windows NT 3.51, and Windows NT 4.00 report
  timestamps on CD-ROMs with respect to local time zones.  If your
  application relies on specific values of timestamps on CDs, please
  send me email and I'll give you some more information.

- Previous versions of CDIMAGE used the ANSI character set for encoding
  filenames.  In V2.10, the default is now the OEM character set, but
  the -c switch will allow you to specify the ANSI character set.

- Previous versions of CDIMAGE separated the -of option for optimizing
  the storage of filenames from the other -o options.  In V2.10, this
  option (-of) is implied when using any form of the -o options.


If you have any problems with CDIMAGE or questions about the options,
please send me mail.  For "shipping product" CDs, please test your
resulting CDs on all appropriate operating systems.  At the very least,
make sure your CDs work with Windows 95, Windows NT 4.0, Windows NT
3.51 (if appropriate), and MS-DOS / Windows 3.x (if appropriate).

Thanks,

Tommcg


