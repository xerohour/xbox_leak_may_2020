@for %%f in (data\*.io)	do @call io2state "%%~nf"
@for %%f in (data_24k\*.io)	do @call io2state24k "%%~nf"
