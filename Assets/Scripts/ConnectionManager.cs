using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

public class ConnectionManager : MonoBehaviour {
    public DebugManager debug;
    public static ConnectionManager Instance;

    public static TcpClient client;
    public static NetworkStream stream ;

    public string ipAddres;
    public int globlaPort;

    public bool[] send;
    public float[] pings;
    protected float timeBetweenPings;

    public int target;
    public bool connected;
    public bool game;
    public bool evenMoreConnected;
    public SceneController scMan;

    // Use this for initialization
    void Awake() {
        if (Instance == null) {
            DontDestroyOnLoad(gameObject);
            Instance = this;
            
        } else if (Instance != this) {
            Destroy(gameObject);
        }
        send = new bool[10];
        pings = new float[10];
        FindSceneManager();
        connected = false;
        evenMoreConnected = false;
        QualitySettings.vSyncCount = 0;
        target = 60;
        Application.targetFrameRate = target;
    }

    public void FindSceneManager() {
        scMan = GameObject.FindWithTag("SceneManager").GetComponent<SceneController>();
    }

    public bool MakeConnection(string ip, int port) {
        try {
            if (!connected) {
                client = new TcpClient(ip, port);
                stream = client.GetStream();
                byte[] data = new byte[5];
                Int32 bytes = stream.Read(data, 0, data.Length);
                String recivedData = Encoding.ASCII.GetString(data, 0, bytes);
                if (recivedData[0] == 'O') {
                    connected = true;
                    ipAddres = ip;
                    globlaPort = port;
                    for(int i =0; i < 10; i++) {
                        send[i] = false;
                    }
                   // StartCoroutine("SendPing");
                } else {
                    return false;
                }
            }
            return true;
        } catch {
            connected = false;
            return false;
        }  
    }


    private void Update() {
        if (Application.targetFrameRate != target)
            Application.targetFrameRate = target;
        if (connected) {
            if (timeBetweenPings <= 0) {
                for (int i = 0; i <= 10; i++) {
                    if (i < 10) {
                        if (!send[i]) {
                            SendPacket(i.ToString());
                            pings[i] = 0.0f;
                            send[i] = true;
                            timeBetweenPings = 1.0f;
                            break;
                        }
                    } else {
                        doDisconnect();
                        scMan.TryToRecconect();
                        connected = false;
                        evenMoreConnected = false;
                    }
                }
            } //else
              //  timeBetweenPings -= Time.deltaTime;

            for (int i = 0; i < 10; i++)
                if (send[i])
                    pings[i] += Time.deltaTime;

           } 
            
    }

    public bool SendName(string name) {
        try {
            if (connected) {
                Byte[] data = Encoding.ASCII.GetBytes(name);
                stream.Write(data, 0, data.Length);
                data = new byte[1];
                String recivedData = "";
                do {
                    Int32 bytes = stream.Read(data, 0, data.Length);
                    recivedData = Encoding.ASCII.GetString(data, 0, bytes);
                    if (recivedData[0] != 'Y' && recivedData[0] != 'U')
                        scMan.DecodeMessage(recivedData);
                } while (recivedData[0] != 'Y' && recivedData[0] != 'U');
                if (recivedData[0] == 'Y') {
                    evenMoreConnected = true;
                    return true;
                }else {
                    return false;
                } 
            } else
                return false;
        } catch {
            return false;
        }
    }

    public void doDisconnect() {
        try {
            stream.Close();
            client.Close();
        }catch {

        }
            connected = false;
            evenMoreConnected = false;
        
    }


    public bool SendPacket(string message) {
        try {
            if (connected) {
                Byte[] data = Encoding.ASCII.GetBytes(message);
                stream.Write(data, 0, data.Length);
            } else
                return false;
            return true;
        } catch {
            return false;
        }
    }

    public float checkPing(int num) {
        send[num]=false;
        return pings[num];
    }

    private void LateUpdate() {
        if (evenMoreConnected) {
            if (stream.CanRead) {
                if (stream.DataAvailable) {
                    byte[] data = new byte[1024];

                    Int32 bytes = stream.Read(data, 0, data.Length);
                    String recivedData = Encoding.ASCII.GetString(data, 0, bytes);
                    String[] dataToDecode = recivedData.Split('\n');
                    foreach (String msg in dataToDecode) {
                        Debug.Log(msg);
                        debug.addLine(msg);
                        try {
                            scMan.DecodeMessage(msg);
                        } catch (Exception e) {
                            debug.addLine("" + e.ToString());
                        }

                    }
                        
                   

                }
                
            }

        }



    }

    public void ToggleGame(bool val = true) {
        game = val;
    }

    private void OnApplicationQuit() {
        doDisconnect();
    }
}
