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
