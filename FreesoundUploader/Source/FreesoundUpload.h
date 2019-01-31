/*
  ==============================================================================

    FreesoundUpload.h
    Created: 25 Jan 2019 6:33:51pm
    Author:  Antonio

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//ClientID = qtRxJcdBeEqAPPymT71w
//ClientSecret = xlMDWbwEp65jNneniFiwNe3u7aKyxBPKrxug05KC

//Login: OPEN IN BROWSER: https://freesound.org/apiv2/oauth2/authorize/?client_id=qtRxJcdBeEqAPPymT71w&response_type=code&state=xyz

//Logout: 


//Using this as a library, the developer has to show and hide this windows



typedef std::pair<int, String> Response;
typedef std::function<void()> AuthorizationCallback;
typedef std::function<void(Response)> ResponseCallback;
typedef std::function<void(int)> ProgressCallback;

class FreesoundRequest : public Thread
{
public:
	FreesoundRequest(URL url, String POSTData, String inAccessToken)
		: Thread("FreesoundUpload"),
		requestedCall(url),
		POST(POSTData),
		accessToken(inAccessToken),
		logger(String(), String())
	{
	}

	~FreesoundRequest() {
		stopThread(100);
	}

	void run() override
	{
		Response result;
		if (progressCallBack == nullptr) { result = makeRequest(); }
		else { result = makeRequestProgress(); }
		if (callBack != nullptr) { 
			MessageManagerLock mml(this);
			if (mml.lockWasGained())
				callBack(result);
			
			 }


	}



	void setProgressCallback(ProgressCallback cb) { progressCallBack = cb; }
	void setResponseCallback(ResponseCallback cb) { callBack = cb; }

	Response makeRequest()
	{
		StringPairArray responseHeaders;
		int statusCode = 0;

		requestedCall = requestedCall.withPOSTData(POST);
		String headers;
		if (accessToken.isNotEmpty()) { headers = "Authorization: Bearer " + accessToken; }

		logger.writeToLog("Response Headers: " + headers);
		logger.writeToLog("url = " + requestedCall.toString(true));


		if (auto stream = std::unique_ptr<InputStream>(requestedCall.createInputStream(true, nullptr, nullptr, headers,
			10000, // timeout in millisecs
			&responseHeaders, &statusCode)))
		{
			String resp = stream->readEntireStreamAsString();
			logger.writeToLog((statusCode != 0 ? "Status code: " + String(statusCode) + newLine : String())
				+ "Response headers: " + newLine
				+ responseHeaders.getDescription() + newLine
				+ "----------------------------------------------------" + newLine
				+ resp);
			return Response(statusCode, resp);
		}

		return Response(-1, String());
	}

	Response makeRequestProgress()
	{
		StringPairArray responseHeaders;
		int statusCode = 0;

		requestedCall = requestedCall.withPOSTData(POST);
		String headers;
		if (accessToken.isNotEmpty()) { headers = "Authorization: Bearer " + accessToken; }

		logger.writeToLog("Response Headers: " + headers);
		logger.writeToLog("url = " + requestedCall.toString(true));

		//URL::OpenStreamProgressCallback* callback = insideProgress;
		if (auto stream = std::unique_ptr<InputStream>(requestedCall.createInputStream(true, nullptr, nullptr, headers,
			10000, // timeout in millisecs
			&responseHeaders, &statusCode)))
		{
			String resp = stream->readEntireStreamAsString();
			logger.writeToLog((statusCode != 0 ? "Status code: " + String(statusCode) + newLine : String())
				+ "Response headers: " + newLine
				+ responseHeaders.getDescription() + newLine
				+ "----------------------------------------------------" + newLine
				+ resp);
			return Response(statusCode, resp);
		}

		return Response(-1, String());
	}

	//THIS FUNCTION IS NOT WORKING
	bool static insideProgress(void* context, int bytesSent, int totalBytes) {

		int progress = (bytesSent / totalBytes) * 100;
		
		static_cast<FreesoundRequest*>(context)->progressCallBack(progress);

		return true;
	}


private:



	URL requestedCall;
	String POST;
	ResponseCallback callBack;
	ProgressCallback progressCallBack;

	String accessToken;
	FileLogger logger;





};

class FreesoundAuthorization : public WebBrowserComponent
{
public:

	//Still have to handle refreshToken

	FreesoundAuthorization(String inClientID, String inClientSecret)
		:	clientID(inClientID),
			clientSecret(inClientSecret),
			authCode(),
			refreshToken()
	{
	}

	~FreesoundAuthorization() {
	}

	void startLogin() {
		String loginURL = "https://freesound.org/apiv2/oauth2/authorize/?client_id=" + clientID + "&response_type=code";
		goToURL(loginURL);
	}

	bool hasAuthentication(){
		return !authCode.isEmpty();
	}

	String getClientID() {
		return clientID;
	}

	String getAuthCode() {
		return authCode;
	}
	
	String getClientSecret() {
		return clientSecret;
	}

	String getAccessToken() {
		return accessToken;
	}

	void setAccessToken(String inAccToken) {
		accessToken = inAccToken;
	}

	String requestAccessToken() {

	}

	void setAuthCallback(AuthorizationCallback cb) { callBack = cb; }

	int codeExchange() {
		URL url = "https://freesound.org/apiv2/oauth2/access_token/";
		String post = "client_id=" + getClientID() + "&client_secret=" + getClientSecret() + "&grant_type=authorization_code&code=" + getAuthCode();
		FreesoundRequest askToken(url, post, String());
		
		Response incoming = askToken.makeRequest();
		if (incoming.first == 200) {
			var answer = JSON::fromString(incoming.second);
			accessToken = answer["access_token"];
			refreshToken = answer["refresh_token"];
		}
		return incoming.first;
	}


private:
	String clientID;
	String clientSecret;
	String authCode;
	String accessToken;
	String refreshToken;
	AuthorizationCallback callBack;

	void pageFinishedLoading(const String &url) {

		//If the page we arrived is step2 of https://freesound.org/docs/api/authentication.html
		if (url.startsWith("https://freesound.org/home/app_permissions/permission_granted/")) {
			//exampleURL = https://freesound.org/home/app_permissions/permission_granted/?code=8zmtRwtflv6Up48QOXlHmlCTblbxth
			//index returns the location of the first char, we have to add 5 to get to the code
			
			URL recUrl(url);
			StringArray paramNames = recUrl.getParameterNames();
			StringArray paramVals = recUrl.getParameterValues();

			int codeInd = paramNames.indexOf("code", true, 0);
			authCode = paramVals[codeInd];

			callBack();
		}
	}

	void pageLoadHadNetworkError() {
		String loginURL = "https://freesound.org/apiv2/oauth2/authorize/?client_id=" + clientID + "&response_type=code";
		goToURL(loginURL);
	}




};

//url = https://freesound.org/apiv2/oauth2/access_token/
//String POSTData = "client_id=" + authorization.getClientID() + "&client_secret=" + authorization.getClientSecret() + "&grant_type=authorization_code&code=" + authorization.getAuthCode();


