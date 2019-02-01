/*
  ==============================================================================

    FreesoundUpload.h
    Created: 25 Jan 2019 6:33:51pm
    Author:  Antonio

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//typedefs used to define the callback functions used
typedef std::pair<int, String> Response;
typedef std::function<void()> AuthorizationCallback;
typedef std::function<void(Response)> ResponseCallback;
typedef std::function<void(int)> ProgressCallback;

//This class is responsible for making a request to the Freesound API.
//The parameters are set when a new FreesoundRequest object is created.
//For each request desired, a new object of this class should be created.
//To better understand what the API has to offer, see https://freesound.org/docs/api/overview.html
class FreesoundRequest : public Thread
{
public:

	//The parameters for the call are set up when creating it.
	//The access token may be an empty string, if no OAauth2 functions are going to be used.
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

	//In case the request is going to be ran in a separate thread.
	//This can be called as object.startThread();.
	void run() override
	{
		Response result;
		//If there is a progress callback, the function that gives progress information is called
		if (progressCallBack == nullptr) { result = makeRequest(); }
		else { result = makeRequestProgress(); }

		//As this piece of code uses the message thread for setting GUI components,
		//only run it if it has a lock on the message thread.
		if (callBack != nullptr) { 
			MessageManagerLock mml(this);
			if (mml.lockWasGained())
				callBack(result);
			 }


	}


	//Functions to be called by the owner of the FSRequest object to set the callbacks.
	//This callback is called whenever there is new information on the progress of the request.
	//Currently this function is not working correctly so it is not supported.
	void setProgressCallback(ProgressCallback cb) { progressCallBack = cb; }
	//This callback is called when the request is answered, and gives the result of the operation.
	void setResponseCallback(ResponseCallback cb) { callBack = cb; }


	//This function is responsible for making the request to FSAPI. After creating the object, if
	//a synchronous request is to be made, call this function. Otherwise call object.startThread();
	Response makeRequest()
	{
		StringPairArray responseHeaders;
		int statusCode = 0;

		//Add the POST data to the URL.
		requestedCall = requestedCall.withPOSTData(POST);
		String headers;

		//If an access token was given, add it to the headers for authentication.
		if (accessToken.isNotEmpty()) { headers = "Authorization: Bearer " + accessToken; }

		//Logging functions
		logger.writeToLog("Response Headers: " + headers);
		logger.writeToLog("url = " + requestedCall.toString(true));


		//Try to open a stream with this information.
		if (auto stream = std::unique_ptr<InputStream>(requestedCall.createInputStream(true, nullptr, nullptr, headers,
			10000, // timeout in millisecs
			&responseHeaders, &statusCode)))
		{
			//Stream created successfully, store the response, log it and return the response in a pair containing (statusCode, response)
			String resp = stream->readEntireStreamAsString();
			logger.writeToLog((statusCode != 0 ? "Status code: " + String(statusCode) + newLine : String())
				+ "Response headers: " + newLine
				+ responseHeaders.getDescription() + newLine
				+ "----------------------------------------------------" + newLine
				+ resp);
			return Response(statusCode, resp);
		}
		//Couldnt create stream, return (-1, emptyString)
		return Response(-1, String());
	}

	//This function is responsible for making the request to FSAPI. After creating the object, if
	//a synchronous request is to be made, call this function. Otherwise call object.startThread();
	//This enables a progress callback but currently is not working
	Response makeRequestProgress()
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

	//Function to call callback and to calculate the percentage of the progress. Is not working.
	bool static insideProgress(void* context, int bytesSent, int totalBytes) {

		int progress = (bytesSent / totalBytes) * 100;
		
		static_cast<FreesoundRequest*>(context)->progressCallBack(progress);

		return true;
	}


private:
	//Parameters which come from the constructor
	URL requestedCall;
	String POST;
	String accessToken;


	//Callback functions
	ResponseCallback callBack;
	ProgressCallback progressCallBack;

	FileLogger logger;
};

//This class handles the authorization process and stores the values needed for
//posterior requests which need authorization. Each application should have an
//object of this class which should be stored until the end of the process.
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

