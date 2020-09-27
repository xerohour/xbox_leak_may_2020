create procedure Cursor_emp$(MACHINE) (@number int = 1) as

declare @cCur int
declare @pchMsg  varchar(128)

declare @empid empid
declare @fname varchar(20)
declare @lname varchar(30)
Declare employee_cursor scroll Cursor
	
	For select fname,lname,emp_id from employee
	Open employee_cursor

select @cCur=0
	while ( @cCur < @number )
	begin
	    select @pchMsg = 'Current Count is:'
		Print @pchMsg
        select @pchMsg = convert(varchar(128), @cCur)
		Print @pchMsg
		Fetch next from employee_cursor into @fname,@lname,@empid
		select @cCur = @cCur + 1
	end

	execute inner_upd$(MACHINE) @empid, @fname, @lname --switch the fname & lname
	fetch relative 0 from employee_cursor into @fname,@lname,@empid
	fetch relative 0 from employee_cursor
	exec inner_upd$(MACHINE) @empid, @fname, @lname --switch it back

	return (0)

