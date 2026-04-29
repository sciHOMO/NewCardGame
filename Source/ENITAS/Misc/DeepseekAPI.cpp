#include "DeepseekAPI.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

FDeepseekAPI::FDeepseekAPI()
{
    APIKey = "";
    Model = "deepseek-chat";
    BaseURL = "https://api.deepseek.com/v1/chat/completions";
}

FDeepseekAPI::~FDeepseekAPI()
{
}

FDeepseekAPI& FDeepseekAPI::Get()
{
    static FDeepseekAPI Instance;
    return Instance;
}

void FDeepseekAPI::SetAPIKey(const FString& Key)
{
    APIKey = Key;
}

void FDeepseekAPI::SetModel(const FString& ModelName)
{
    Model = ModelName;
}

void FDeepseekAPI::SendChatRequest(const FString& Prompt, FOnDeepseekResponse Callback)
{
    TArray<TPair<FString, FString>> Messages;
    Messages.Add(TPair<FString, FString>("user", Prompt));
    SendChatRequestWithHistory(Messages, Callback);
}

void FDeepseekAPI::SendChatRequestWithHistory(const TArray<TPair<FString, FString>>& Messages, FOnDeepseekResponse Callback)
{
    if (APIKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Deepseek API Key is not set!"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseURL);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *APIKey));

    FString RequestBody = BuildRequestBody(Messages);
    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindRaw(this, &FDeepseekAPI::OnRequestComplete, Callback);
    Request->ProcessRequest();
}

FString FDeepseekAPI::BuildRequestBody(const TArray<TPair<FString, FString>>& Messages)
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
    RootObject->SetStringField("model", Model);
    
    TArray<TSharedPtr<FJsonValue>> MessageArray;
    for (const TPair<FString, FString>& Msg : Messages)
    {
        TSharedPtr<FJsonObject> MsgObj = MakeShareable(new FJsonObject);
        MsgObj->SetStringField("role", Msg.Key);
        MsgObj->SetStringField("content", Msg.Value);
        MessageArray.Add(MakeShareable(new FJsonValueObject(MsgObj)));
    }
    RootObject->SetArrayField("messages", MessageArray);
    RootObject->SetNumberField("temperature", 0.7);
    RootObject->SetNumberField("max_tokens", 4096);

    FString OutputString;
    TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<TCHAR>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
    
    return OutputString;
}

void FDeepseekAPI::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnDeepseekResponse Callback)
{
    FString ResponseContent = "";
    
    if (bWasSuccessful && Response.IsValid())
    {
        if (Response->GetResponseCode() == 200)
        {
            ResponseContent = Response->GetContentAsString();
            UE_LOG(LogTemp, Log, TEXT("Deepseek API Response: %s"), *ResponseContent);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Deepseek API Error: HTTP %d"), Response->GetResponseCode());
            ResponseContent = FString::Printf(TEXT("HTTP Error: %d"), Response->GetResponseCode());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Deepseek API Request Failed"));
        ResponseContent = "Request Failed";
    }

    if (Callback.IsBound())
    {
        Callback.Execute(ResponseContent);
    }
}

FString FDeepseekAPI::ParseResponseContent(const FString& ResponseJson)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ResponseJson);
    
    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        const TArray<TSharedPtr<FJsonValue>>* Choices;
        if (JsonObject->TryGetArrayField("choices", Choices) && Choices->Num() > 0)
        {
            const TSharedPtr<FJsonObject>* ChoiceObj;
            if ((*Choices)[0]->TryGetObject(ChoiceObj))
            {
                const TSharedPtr<FJsonObject>* MessageObj;
                if ((*ChoiceObj)->TryGetObjectField("message", MessageObj))
                {
                    FString Content;
                    if ((*MessageObj)->TryGetStringField("content", Content))
                    {
                        return Content;
                    }
                }
            }
        }
    }
    
    return "";
}

FString FDeepseekAPI::GetErrorMessage(const FString& ResponseJson)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ResponseJson);
    
    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        const TSharedPtr<FJsonObject>* ErrorObj;
        if (JsonObject->TryGetObjectField("error", ErrorObj))
        {
            FString Message;
            if ((*ErrorObj)->TryGetStringField("message", Message))
            {
                return Message;
            }
        }
    }
    
    return "";
}
