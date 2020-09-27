if exists (select * from sysobjects where id = object_id('$(USER).inner_upd$(MACHINE)') and sysstat & 0xf = 4)
	drop procedure $(USER).inner_upd$(MACHINE)
GO

if exists (select * from sysobjects where id = object_id('$(USER).Cursor_emp$(MACHINE)') and sysstat & 0xf = 4)
	drop procedure $(USER).Cursor_emp$(MACHINE)

