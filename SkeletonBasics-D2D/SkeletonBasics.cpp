//------------------------------------------------------------------------------
// <copyright file="SkeletonBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <strsafe.h>
#include "SkeletonBasics.h"
#include "resource.h"
#include <iostream>
#include <fstream>

using namespace std;

static const float g_JointThickness = 3.0f;
static const float g_TrackedBoneThickness = 6.0f;
static const float g_InferredBoneThickness = 1.0f;

int com = 0, follow = 0;
const int n = 7;
double a[n], b[n], an_m[100][n], last[n], fi[n];
int iter_i[100][n];
double iter_f = 0, iter_a;
int num[n] = { 0 };
int iter = 0, bo[n];


bool check() {
	for (int i = 0; i < n; i++)
	{
		if (abs(a[i] - b[i]) * 20 > a[i]) return false;
	}
	return true;
}

/*int max(int a, int b){
	if (a >= b) return a;
	else return b;
}

double max(double a, double b) {
	if (a >= b) return a;
	else return b;
}*/

void reset(double b[]) {
	for (int i = 0; i < n; i++) {
		b[i] = 0;
	}
}

void reset(int b[]) {
	for (int i = 0; i < n; i++) {
		b[i] = 0;
	}
}

void reset2_an_m() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < 100; j++) {
			an_m[j][i] = 0;
		}
	}
}

void reset2_iter_i() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < 100; j++) {
			iter_i[j][i] = 0;
		}
	}
}

void set_f(double ar[])
{
	for (int i = 0; i<n; i++) {
		for (int j = 1; j<100; j++) {
			fi[i] += an_m[j][i];
		}
		fi[i] /= num[i];
	}
	for (int j = 1; j<100; j++) {
		if (iter_i[j] == 0) iter_f = (iter_i[j - 1] - iter_i[1]) / j - 2;
	}
	if (iter_f == 0) iter_f = (iter_i[99] - iter_i[1]) / 98.0;
}

void set_arr(double a[], double b[])
{
	for (int i = 0; i<n; i++) {
		a[i] = b[i];
	}
}

double distance(Vector4 x, Vector4 y) {
	return sqrt(pow(x.x - y.x, 2) + pow(x.y - y.y, 2) + pow(x.z - y.z, 2));
}

void g(Vector4 x, Vector4 y, int k, double a[])
{
	if (bo[k] == 1 && last[k]>distance(x, y)) {
		bo[k] = 0;
		an_m[num[k]][k] = a[k];
		iter_i[num[k]][k] = iter;
		num[k]++;
		reset(a);
	}
	if (bo[k] == 0 && last[k]<distance(x, y)) {
		bo[k] = 1;
	}
}

void write() {
	ofstream outfile;// declaration of file pointer named outfile
	outfile.open("filename.txt", ios::out); // opens file named "filename" for output
	for (int i = 0; i < n; i++) {
		outfile << a[i] << endl;
	}
	outfile.close();
}

void read() {
	fstream infile;
	double o[7];
	int i = 0;
	while (i < 7) {
		infile >> o[i];
		a[i] = o[i];
		i++;
	}
}

void close_a() {
	set_f(a); set_arr(a, fi); iter_a = iter_f; reset(last); reset(fi); reset(num); reset(bo); iter = 0; iter_f = 0; reset2_an_m(); reset2_iter_i(); write();
}

void close_b() {
	set_f(b); if(check()) follow = 1; set_arr(a, fi); iter_a = iter_f; reset(last); reset(fi); reset(num); reset(bo); iter = 0; iter_f = 0; reset2_an_m(); reset2_iter_i();
}

void setb(const Vector4 c[])
{
	iter++;
	if (iter > 300) close_b();
	if (num[0] >= 3 && num[1] >= 3 && num[2] >= 3 && num[3] >= 3 && num[4] >= 3 && num[5] >= 3 && num[6] >= 3) close_b();
	if (num[0]<3) { g(c[3], c[14], 0, b); b[0] = max(b[0], distance(c[3], c[14])); last[0] = distance(c[3], c[14]); }
	if (num[1]<3) { g(c[3], c[18], 1, b); b[1] = max(b[1], distance(c[3], c[18])); last[1] = distance(c[3], c[18]); }
	if (num[2]<3) { g(c[14], c[18], 2, b); b[2] = max(b[2], distance(c[14], c[18])); last[2] = distance(c[14], c[18]); }
	if (num[3]<3) { g(c[6], c[10], 3, b); b[3] = max(b[3], distance(c[6], c[10])); last[3] = distance(c[6], c[10]); }
	if (num[4]<3) { g(c[1], c[10], 4, b); b[4] = max(b[4], distance(c[1], c[10])); last[4] = distance(c[1], c[10]); }
	if (num[5]<3) { g(c[1], c[6], 5, b); b[5] = max(b[5], distance(c[1], c[6])); last[5] = distance(c[1], c[6]); }
	if (num[6]<3) { g(c[13], c[17], 6, b); b[6] = max(b[6], distance(c[13], c[17])); last[6] = distance(c[13], c[17]); }
}

void seta(const Vector4 c[])
{
	iter++;
	if (iter > 2000) close_a();
	else if (num[0] >= 20 && num[1] >= 20 && num[2] >= 20 && num[3] >= 20 && num[4] >= 20 && num[5] >= 20 && num[6] >= 20) close_a();
	if (num[0]<20) { g(c[3], c[14], 0, a); a[0] = max(a[0], distance(c[3], c[14])); last[0] = distance(c[3], c[14]); }
	if (num[1]<20) { g(c[3], c[18], 1, a); a[1] = max(a[1], distance(c[3], c[18])); last[1] = distance(c[3], c[18]); }
	if (num[2]<20) { g(c[14], c[18], 2, a); a[2] = max(a[2], distance(c[14], c[18])); last[2] = distance(c[14], c[18]); }
	if (num[3]<20) { g(c[6], c[10], 3, a); a[3] = max(a[3], distance(c[6], c[10])); last[3] = distance(c[6], c[10]); }
	if (num[4]<20) { g(c[1], c[10], 4, a); a[4] = max(a[4], distance(c[1], c[10])); last[4] = distance(c[1], c[10]); }
	if (num[5]<20) { g(c[1], c[6], 5, a); a[5] = max(a[5], distance(c[1], c[6])); last[5] = distance(c[1], c[6]); }
	if (num[6]<20) { g(c[13], c[17], 6, a); a[6] = max(a[6], distance(c[13], c[17])); last[6] = distance(c[13], c[17]); }
}

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    CSkeletonBasics application;
    application.Run(hInstance, nCmdShow);
}

/// <summary>
/// Constructor
/// </summary>
CSkeletonBasics::CSkeletonBasics() :
    m_pD2DFactory(NULL),
    m_hNextSkeletonEvent(INVALID_HANDLE_VALUE),
    m_pSkeletonStreamHandle(INVALID_HANDLE_VALUE),
    m_bSeatedMode(false),
    m_pRenderTarget(NULL),
    m_pBrushJointTracked(NULL),
    m_pBrushJointInferred(NULL),
    m_pBrushBoneTracked(NULL),
    m_pBrushBoneInferred(NULL),
    m_pNuiSensor(NULL)
{
    ZeroMemory(m_Points,sizeof(m_Points));
}

/// <summary>
/// Destructor
/// </summary>
CSkeletonBasics::~CSkeletonBasics()
{
    if (m_pNuiSensor)
    {
		if(com == 0) close_a();
		else close_b();
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }

    // clean up Direct2D objects
    DiscardDirect2DResources();

    // clean up Direct2D
    SafeRelease(m_pD2DFactory);

    SafeRelease(m_pNuiSensor);
}

/// MAnipulate here
/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CSkeletonBasics::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc  = {0};

    // Dialog custom window class
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"SkeletonBasicsAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CSkeletonBasics::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    const int eventCount = 1;
    HANDLE hEvents[eventCount];

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        hEvents[0] = m_hNextSkeletonEvent;

        // Check to see if we have either a message (by passing in QS_ALLEVENTS)
        // Or a Kinect event (hEvents)
        // Update() will check for Kinect events individually, in case more than one are signalled
        MsgWaitForMultipleObjects(eventCount, hEvents, FALSE, INFINITE, QS_ALLINPUT);

        // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
        // can return for other reasons even though it is signaled.
        Update();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}


/// <summary>
/// Main processing function
/// </summary>
void CSkeletonBasics::Update()
{
    if (NULL == m_pNuiSensor)
    {
        return;
    }

    // Wait for 0ms, just quickly test if it is time to process a skeleton
    if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
    {
        ProcessSkeleton();
    }
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CSkeletonBasics::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CSkeletonBasics* pThis = NULL;

    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CSkeletonBasics*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CSkeletonBasics*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CSkeletonBasics::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

            // Look for a connected Kinect, and create it if found
            CreateFirstConnected();
        }
        break;

        // If the titlebar X is clicked, destroy app
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        // Quit the main message pump
        PostQuitMessage(0);
        break;

        // Handle button press
    case WM_COMMAND:
        // If it was for the near mode control and a clicked event, change near mode
        if (IDC_CHECK_SEATED == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        {
            // Toggle out internal state for near mode
            m_bSeatedMode = !m_bSeatedMode;

            if (NULL != m_pNuiSensor)
            {
                // Set near mode for sensor based on our internal state
                m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, m_bSeatedMode ? NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT : 0);
            }
        }
        break;
    }

    return FALSE;
}

/// <summary>
/// Create the first connected Kinect found 
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CSkeletonBasics::CreateFirstConnected()
{
    INuiSensor * pNuiSensor;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when skeleton data is available
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
        SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

/// <summary>
/// Handle new skeleton data
/// </summary>
void CSkeletonBasics::ProcessSkeleton()
{
    NUI_SKELETON_FRAME skeletonFrame = {0};

    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

    // Endure Direct2D is ready to draw
    hr = EnsureDirect2DResources( );
    if ( FAILED(hr) )
    {
        return;
    }

    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->Clear( );

    RECT rct;
    GetClientRect( GetDlgItem( m_hWnd, IDC_VIDEOVIEW ), &rct);
    int width = rct.right;
    int height = rct.bottom;

    for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
    {
        NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

        if (NUI_SKELETON_TRACKED == trackingState)
        {
            // We're tracking the skeleton, draw it
            DrawSkeleton(skeletonFrame.SkeletonData[i], width, height);
        }
        else if (NUI_SKELETON_POSITION_ONLY == trackingState)
        {
            // we've only received the center point of the skeleton, draw that
            D2D1_ELLIPSE ellipse = D2D1::Ellipse(
                SkeletonToScreen(skeletonFrame.SkeletonData[i].Position, width, height),
                g_JointThickness,
                g_JointThickness
                );

            m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointTracked);
        }
    }

    hr = m_pRenderTarget->EndDraw();

    // Device lost, need to recreate the render target
    // We'll dispose it now and retry drawing
    if (D2DERR_RECREATE_TARGET == hr)
    {
        hr = S_OK;
        DiscardDirect2DResources();
    }
}

/// <summary>
/// Draws a skeleton
/// </summary>
/// <param name="skel">skeleton to draw</param>
/// <param name="windowWidth">width (in pixels) of output buffer</param>
/// <param name="windowHeight">height (in pixels) of output buffer</param>
void CSkeletonBasics::DrawSkeleton(const NUI_SKELETON_DATA & skel, int windowWidth, int windowHeight)
{      
    int i;

    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
        m_Points[i] = SkeletonToScreen(skel.SkeletonPositions[i], windowWidth, windowHeight);
    }

	if (com == 0) seta(skel.SkeletonPositions);
	else if (follow == 0) { setb(skel.SkeletonPositions); read(); }

    // Render Torso
    DrawBone(skel, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SPINE);
    DrawBone(skel, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_HIP_CENTER);
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT);

    // Left Arm
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);

    // Right Arm
    DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);

    // Left Leg
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT);
    DrawBone(skel, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);

    // Right Leg
    DrawBone(skel, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT);
    DrawBone(skel, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);

    // Draw the joints in a different color
    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse( m_Points[i], g_JointThickness, g_JointThickness );

        if ( skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_INFERRED )
        {
            m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointInferred);
        }
        else if ( skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_TRACKED )
        {
            m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointTracked);
        }
    }
}

/// <summary>
/// Draws a bone line between two joints
/// </summary>
/// <param name="skel">skeleton to draw bones from</param>
/// <param name="joint0">joint to start drawing from</param>
/// <param name="joint1">joint to end drawing at</param>
void CSkeletonBasics::DrawBone(const NUI_SKELETON_DATA & skel, NUI_SKELETON_POSITION_INDEX joint0, NUI_SKELETON_POSITION_INDEX joint1)
{
    NUI_SKELETON_POSITION_TRACKING_STATE joint0State = skel.eSkeletonPositionTrackingState[joint0];
    NUI_SKELETON_POSITION_TRACKING_STATE joint1State = skel.eSkeletonPositionTrackingState[joint1];

    // If we can't find either of these joints, exit
    if (joint0State == NUI_SKELETON_POSITION_NOT_TRACKED || joint1State == NUI_SKELETON_POSITION_NOT_TRACKED)
    {
        return;
    }

    // Don't draw if both points are inferred
    if (joint0State == NUI_SKELETON_POSITION_INFERRED && joint1State == NUI_SKELETON_POSITION_INFERRED)
    {
        return;
    }

    // We assume all drawn bones are inferred unless BOTH joints are tracked
    if (joint0State == NUI_SKELETON_POSITION_TRACKED && joint1State == NUI_SKELETON_POSITION_TRACKED)
    {
        m_pRenderTarget->DrawLine(m_Points[joint0], m_Points[joint1], m_pBrushBoneTracked, g_TrackedBoneThickness);
    }
    else
    {
        m_pRenderTarget->DrawLine(m_Points[joint0], m_Points[joint1], m_pBrushBoneInferred, g_InferredBoneThickness);
    }
}

/// <summary>
/// Converts a skeleton point to screen space
/// </summary>
/// <param name="skeletonPoint">skeleton point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F CSkeletonBasics::SkeletonToScreen(Vector4 skeletonPoint, int width, int height)
{
    LONG x, y;
    USHORT depth;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);

    float screenPointX = static_cast<float>(x * width) / cScreenWidth;
    float screenPointY = static_cast<float>(y * height) / cScreenHeight;

    return D2D1::Point2F(screenPointX, screenPointY);
}

/// <summary>
/// Ensure necessary Direct2d resources are created
/// </summary>
/// <returns>S_OK if successful, otherwise an error code</returns>
HRESULT CSkeletonBasics::EnsureDirect2DResources()
{
    HRESULT hr = S_OK;

    // If there isn't currently a render target, we need to create one
    if (NULL == m_pRenderTarget)
    {
        RECT rc;
        GetWindowRect( GetDlgItem( m_hWnd, IDC_VIDEOVIEW ), &rc );  

        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        D2D1_SIZE_U size = D2D1::SizeU( width, height );
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a Hwnd render target, in order to render to the window set in initialize
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(GetDlgItem( m_hWnd, IDC_VIDEOVIEW), size),
            &m_pRenderTarget
            );
        if ( FAILED(hr) )
        {
            SetStatusMessage(L"Couldn't create Direct2D render target!");
            return hr;
        }

        //light green
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);

        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pBrushBoneTracked);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &m_pBrushBoneInferred);
    }

    return hr;
}

/// <summary>
/// Dispose Direct2d resources 
/// </summary>
void CSkeletonBasics::DiscardDirect2DResources( )
{
    SafeRelease(m_pRenderTarget);

    SafeRelease(m_pBrushJointTracked);
    SafeRelease(m_pBrushJointInferred);
    SafeRelease(m_pBrushBoneTracked);
    SafeRelease(m_pBrushBoneInferred);
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void CSkeletonBasics::SetStatusMessage(WCHAR * szMessage)
{
    SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
}