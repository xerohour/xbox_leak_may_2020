/****** Object:  Table sales$(MACHINE)    Script Date: 9/11/97 11:46:29 PM ******/
declare @salesdiagramid int;

select @salesdiagramid = objectid from dtproperties where value = 'sales$(MACHINE) diagram'
delete from dtproperties where @salesdiagramid = objectid

GO

if exists (select * from sysobjects where id = object_id('sales$(MACHINE)') and sysstat & 0xf = 3)
	drop table "sales$(MACHINE)"
GO

if exists (select * from sysobjects where id = object_id('sales$(MACHINE) view') and sysstat & 0xf = 2)
	drop view "sales$(MACHINE) view"
GO


--setuser 'dbo'
--GO

setuser
GO


/****** Object:  Table sales$(MACHINE)    Script Date: 9/11/97 11:46:29 PM ******/
CREATE TABLE "sales$(MACHINE)" (
	stor_id char (4) NOT NULL ,
	ord_num varchar (20) NOT NULL ,
	ord_date datetime NOT NULL ,
	qty smallint NOT NULL ,
	payterms varchar (12) NOT NULL ,
	title_id tid NOT NULL ,
	CONSTRAINT "UPKCL_sales$(MACHINE)" PRIMARY KEY  CLUSTERED 
	(
		stor_id,
		ord_num,
		title_id
	)
)
GO

CREATE  INDEX titleidind$(MACHINE) ON "sales$(MACHINE)"(title_id)
GO


raiserror('Now at the inserts to sales$(MACHINE) ....',1,1)

GO

insert "sales$(MACHINE)" values('7066', 'QA7442.3', '09/13/94', 75, 'ON invoice','PS2091')
insert "sales$(MACHINE)" values('7067', 'D4482', '09/14/94', 10, 'Net 60','PS2091')
insert "sales$(MACHINE)" values('7131', 'N914008', '09/14/94', 20, 'Net 30','PS2091')
insert "sales$(MACHINE)" values('7131', 'N914014', '09/14/94', 25, 'Net 30','MC3021')
insert "sales$(MACHINE)" values('8042', '423LL922', '09/14/94', 15, 'ON invoice','MC3021')
insert "sales$(MACHINE)" values('8042', '423LL930', '09/14/94', 10, 'ON invoice','BU1032')
insert "sales$(MACHINE)" values('6380', '722a', '09/13/94', 3, 'Net 60','PS2091')
insert "sales$(MACHINE)" values('6380', '6871', '09/14/94', 5, 'Net 60','BU1032')
insert "sales$(MACHINE)" values('8042','P723', '03/11/93', 25, 'Net 30', 'BU1111')
insert "sales$(MACHINE)" values('7896','X999', '02/21/93', 35, 'ON invoice', 'BU2075')
insert "sales$(MACHINE)" values('7896','QQ2299', '10/28/93', 15, 'Net 60', 'BU7832')
insert "sales$(MACHINE)" values('7896','TQ456', '12/12/93', 10, 'Net 60', 'MC2222')
insert "sales$(MACHINE)" values('8042','QA879.1', '5/22/93', 30, 'Net 30', 'PC1035')
insert "sales$(MACHINE)" values('7066','A2976', '5/24/93', 50, 'Net 30', 'PC8888')
insert "sales$(MACHINE)" values('7131','P3087a', '5/29/93', 20, 'Net 60', 'PS1372')
insert "sales$(MACHINE)" values('7131','P3087a', '5/29/93', 25, 'Net 60', 'PS2106')
insert "sales$(MACHINE)" values('7131','P3087a', '5/29/93', 15, 'Net 60', 'PS3333')
insert "sales$(MACHINE)" values('7131','P3087a', '5/29/93', 25, 'Net 60', 'PS7777')
insert "sales$(MACHINE)" values('7067','P2121', '6/15/92', 40, 'Net 30', 'TC3218')
insert "sales$(MACHINE)" values('7067','P2121', '6/15/92', 20, 'Net 30', 'TC4203')
insert "sales$(MACHINE)" values('7067','P2121', '6/15/92', 20, 'Net 30', 'TC7777')

GO
