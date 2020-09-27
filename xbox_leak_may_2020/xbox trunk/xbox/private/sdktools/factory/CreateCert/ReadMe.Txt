Copyright 1998-1999. Microsoft Corporation. All Right Reserved.



CreateCert takes the following arguments:

CreateCert [Import option] <X509 Name> [Options]

[Import Option]
-i <CertFile> <KeyFile> <Password> <store> <u|m> <Provider num>

    When -i is used no other options should be used.  This option
    is used to import a certificate along with its private key.
    <CertFile> - Certificate File
    <KeyFile>  - Private/Public Key File
    <Password> - Password used to decode Key File
    <store>    - Store where certificate will be placed
    <u|m>      - Local of Certificate.  Either the User Store
                     or the Machine Store
    <Provider> - Number used to indicate the provider.  Type
                 CreateCert to get the Usage along with the
                 provider numbers.

<X509 Name>

    This is the Name of the Certificate in X509 format. For Example:
    "CN=Certificate,OU=Support,O=Company X,L=Seattle,S=Washington"

    Common Name:        Certificate
    Organization Unit:  Support
    Organization:       Company X
    Locale:             Seattle
    State:              Washington

    "CN=Bill's Certificate,S=California,C=US,E=Bill@mail.net"

    Common Name :       Bill's Certificate
    State :             California
    Country:            US
    Email:              Bill@mail.net

[Options]
-p <provider number>

    Cryptographic Provider Number: Given to user by Usage.

-k  <e>xchange|<s>ignature

    Type of Key - Exchange Key or Signature Key
                  Default is an Exchange Key

-s <sha>|<md5> - Hash algorithm of Certificate

-m <number of months of validity>

    The number of months the certificate will be valid for.
    Self-Signed Certificates default to 24 months.
    Non Self-Signed Certificates default to 6 months.

-ca

    A Basic Contstraints extension is always added to the certificate.
    This extension specifies whether the certificate will be a
    Certificate Authority or not.  Self-Signed are always marked as
    CA so the flag isn't needed in that case.

-u <enhanced key usage number>

    This is a number indicates what enhanced key usage extension the
    Certificate will have.  Add the numbers below to get the final
    number.
    Server Authentication : 1
    Client Authentication : 2
    Code Signing          : 4
    Email Protection      : 8
    Time Stamping         : 16

-sl <store> <u>|<m>

    Specifies the Store of the Certificate.  Default is MY u meaning
    it will be places in the User "MY" store.  If you specify this
    option don't use the -ex option.

-ex <CertFile> <KeyFile> <Password>

    This option is to export the certificate and the Private Key
    Information. The KeyFile is encrypted using the password.
    If you use this option don't use the -sl option.

-is <Substring of Issuer Certificate Name> <store> <u>|<m>

    This specifies what Issuer Certificate will sign the certificate
    being created.  The Issuer Certificate must have a private key
    pair associated with it to sign the Certificate being created.  If
    this option is not used a Self-Signed Certificate is created.
