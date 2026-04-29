#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

DECLARE_DELEGATE_OneParam(FOnDeepseekResponse, const FString&);

class ENITAS_API FDeepseekAPI
{
public:
    static FDeepseekAPI& Get();

    void SetAPIKey(const FString& Key);
    void SetModel(const FString& ModelName);
    
    void SendChatRequest(const FString& Prompt, FOnDeepseekResponse Callback);
    void SendChatRequestWithHistory(const TArray<TPair<FString, FString>>& Messages, FOnDeepseekResponse Callback);

    FString ParseResponseContent(const FString& ResponseJson);
    FString GetErrorMessage(const FString& ResponseJson);

private:
    FDeepseekAPI();
    ~FDeepseekAPI();

    FDeepseekAPI(const FDeepseekAPI&) = delete;
    FDeepseekAPI& operator=(const FDeepseekAPI&) = delete;

    FString APIKey;
    FString Model;
    FString BaseURL;

    void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnDeepseekResponse Callback);
    FString BuildRequestBody(const TArray<TPair<FString, FString>>& Messages);
};
