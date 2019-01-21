using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DebugManager : MonoBehaviour
{
    public static DebugManager Instance;

    public Text debugField;
    public GameObject debugWindow;
    public InputField commandLine;
    public SceneController scMen;
    public ConnectionManager cm;
    bool debugOn;

    private void Awake() {
        if (Instance == null) {
            DontDestroyOnLoad(gameObject);
            Instance = this;
            cm = GameObject.FindWithTag("ConnectionManager").GetComponent<ConnectionManager>();
        } else if (Instance != this) {
            Destroy(gameObject);
        }

    }

    public void ToggleDebugWindow() {
        if (debugWindow.activeSelf)
            debugWindow.SetActive(false);
        else
            debugWindow.SetActive(true);
    }

    public void addLine(string msg) {
        debugField.rectTransform.sizeDelta = new Vector2(debugField.rectTransform.sizeDelta.x, debugField.rectTransform.sizeDelta.y + 33);
        debugField.text +=  msg + "\n";
    }

    void Update()
    {
        if(scMen = null)
            scMen = GameObject.FindWithTag("SceneManager").GetComponent<SceneController>();

        if (Input.GetKey(KeyCode.LeftShift) && Input.GetKey(KeyCode.LeftControl) && Input.GetKey(KeyCode.D)) {
            if (!debugOn) {
                debugOn = true;
                ToggleDebugWindow();
            }
        }else {
            debugOn = false;
        }

        if (commandLine.isFocused && Input.GetKey(KeyCode.Return) && commandLine.text != "") {
            string comand = commandLine.text;
            string[] com = comand.Split(':');
            if (com[0] == "Send") {
                cm.SendPacket(com[1]);
                addLine("Send: " + commandLine.text);
            } else if(com[0] == "Fps") {
                cm.target = int.Parse(com[1]);
            }

            commandLine.text = "";
        }
    }
}
