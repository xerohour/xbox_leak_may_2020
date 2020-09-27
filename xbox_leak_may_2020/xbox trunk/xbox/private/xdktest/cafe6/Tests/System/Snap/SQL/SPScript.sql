if exists (select * from sysobjects where id = object_id('$USER).inner_upd$(MACHINE)') and sysstat & 0xf = 4)
	drop procedure $(USER).inner_upd$(MACHINE)
GO

CREATE PROCEDURE inner_upd$(MACHINE)
@id empid=NULL,
@firstname varchar(20)=NULL,
@lastname varchar(20)=NULL

AS 
if(@id=NULL)
update employee set 
	fname=NULL,
	lname=NULL
	where 	fname=@firstname and lname=@lastname

else --switch the names
update employee set 
	lname=@firstname,
	fname=@lastname
	where emp_id=@id

print 'Switched first and last name of employee...'
print @id

	RETURN

GO



if exists (select * from sysobjects where id = object_id('$(USER).Cursor_emp$(MACHINE)') and sysstat & 0xf = 4)
	drop procedure $(USER).Cursor_emp$(MACHINE)
GO

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

