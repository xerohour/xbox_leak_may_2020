b$(TARGET): 
	cmd /c "cd $(TARGET) && nmake /f $(TARGET).mak"

c$(TARGET):
	-cmd /c "cd $(TARGET)\debug && del /q *.*"
