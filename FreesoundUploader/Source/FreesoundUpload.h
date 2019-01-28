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


typedef std::function<void()> AuthorizationCallback;
typedef std::function<void(String)> ResponseCallback;



class FreesoundAuthorization : public WebBrowserComponent
{
	FreesoundAuthorization(String inClientID, String inClientSecret, AuthorizationCallback cb)
		:	clientID(inClientID),
			clientSecret(inClientSecret),
			authCode(),
			callBack(cb)
	{
	}

	~FreesoundAuthorization() {

	}

	bool startLogin() {
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


private:
	String clientID;
	String clientSecret;
	String authCode;
	String accessToken;
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

			//callback saying it has finished
		}
	}

	void pageLoadHadNetworkError() {
		String loginURL = "https://freesound.org/apiv2/oauth2/authorize/?client_id=" + clientID + "&response_type=code";
		goToURL(loginURL);
	}

	void codeExchange(){}


};

//url = https://freesound.org/apiv2/oauth2/access_token/
//String POSTData = "client_id=" + authorization.getClientID() + "&client_secret=" + authorization.getClientSecret() + "&grant_type=authorization_code&code=" + getAuthCode();


class FreesoundUpload : private Thread
{
public:
	FreesoundUpload(URL url, String POSTData, FreesoundAuthorization &auth, ResponseCallback cb)
		: Thread("FreesoundUpload"),
		requestedCall(url),
		authorization(auth),
		POST(POSTData),
		callBack(cb)
	{
	}

	void run() override
	{
		String result = getAccessToken(requestedCall, POST, authorization);
		if (callBack != nullptr) { callBack(result); }

	}

private:



	URL requestedCall;
	String POST;
	FreesoundAuthorization &authorization;
	ResponseCallback callBack;



	String getAccessToken(URL url, const String POSTdata, FreesoundAuthorization &authorization)
	{
		StringPairArray responseHeaders;
		int statusCode = 0;

		url = url.withPOSTData(POSTdata);

		if (auto stream = std::unique_ptr<InputStream>(url.createInputStream(true, nullptr, nullptr, {},
			10000, // timeout in millisecs
			&responseHeaders, &statusCode)))
		{




			//authorization.setAccessToken(accToken);

			return (statusCode != 0 ? "Status code: " + String(statusCode) + newLine : String())
				+ "Response headers: " + newLine
				+ responseHeaders.getDescription() + newLine
				+ "----------------------------------------------------" + newLine
				+ stream->readEntireStreamAsString();
		}

		if (statusCode != 0)
			return "Failed to connect, status code = " + String(statusCode);

		return "Failed to connect!";
	}


};