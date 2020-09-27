
extern LPSTR SignatureRpcEndpointName;
extern LPSTR SignatureRpcEndpointBindingString;

BOOL
ConnectToSignatureServer(
    LPSTR LoggingText
    );

BOOL
GenerateSignatureFromHashValue(
    IN  PMD5_HASH HashValue,
    IN  LPSTR     LoggingText,
    OUT PDWORD    SignatureId,
    OUT PUCHAR    Signature
    );


