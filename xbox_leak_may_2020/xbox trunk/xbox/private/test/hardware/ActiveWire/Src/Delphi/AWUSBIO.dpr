program AWUSBIO;

uses
  Forms,
  AWUSB in 'AWUSB.pas' {frmMain},
  AwusbApi in 'AwusbApi.pas',
  AwusbAbout in 'AwusbAbout.pas' {frmAbout};

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TfrmMain, frmMain);
  Application.CreateForm(TfrmAbout, frmAbout);
  Application.Run;
end.
