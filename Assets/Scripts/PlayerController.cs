using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour {

    public float bombTimer;

    public enum direction { up, down, left, right, stay};
    Player stats;
    GlobalController gc;
    GameController gamec;
    public direction nextMove;
    protected Vector3 currentPos;
    protected Vector3 nextPosition;
    protected float isMoving;

	// Use this for initialization
	void Start () {
        gc = GameObject.FindWithTag("GlobalController").GetComponent<GlobalController>();
        gamec = GameObject.FindWithTag("SceneManager").GetComponent<GameController>();
        stats = gameObject.GetComponent<Player>();
        bombTimer = 0;
        nextMove = direction.stay;
        isMoving = 0;
	}

    public void MakeDead(int own) {
        if (!gc.ended)
            if (!stats.amiDead()) {
                gamec.cm.SendPacket("K" + own.ToString());
                stats.setDead(true);
            }
    }
	
	// Update is called once per frame
	void Update () {
        if (!gc.ended && !gamec.reconect) {
            if (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow)) {
                if (isMoving < 0.15f)

                    nextMove = direction.up;
            } else if (Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow)) {
                if (isMoving < 0.15f)

                    nextMove = direction.down;
            }

            if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow)) {
                if (isMoving < 0.15f)

                    nextMove = direction.left;
            } else if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow)) {
                if (isMoving < 0.15f)

                    nextMove = direction.right;
            }



            if (isMoving <= 0 && nextMove != direction.stay) {
                currentPos = transform.position;
                nextPosition = currentPos;
                isMoving = 0.25f;
                switch (nextMove) {
                    case direction.up:
                        if (Mathf.Floor(transform.position.z) < gc.gameMap.getSize('y'))
                            nextPosition = new Vector3(currentPos.x, currentPos.y, currentPos.z + 1);
                        break;
                    case direction.down:
                        if (Mathf.Round(transform.position.z) > 0)
                            nextPosition = new Vector3(currentPos.x, currentPos.y, currentPos.z - 1);
                        break;
                    case direction.left:
                        if (Mathf.Round(transform.position.x) > 0)
                            nextPosition = new Vector3(currentPos.x - 1, currentPos.y, currentPos.z);
                        break;
                    case direction.right:
                        if (Mathf.Round(transform.position.x) < gc.gameMap.getSize('x'))
                            nextPosition = new Vector3(currentPos.x + 1, currentPos.y, currentPos.z);
                        break;
                }
                nextMove = direction.stay;
                if (!gc.gameMap.checkIfWalkable((int)nextPosition.x, (int)nextPosition.z))
                    isMoving = 0.0f;
                else {
                    gamec.SendMyMove((int)nextPosition.x, (int)nextPosition.z);
                }

            } else if (isMoving > 0) {
                isMoving -= Time.deltaTime;
                float complete = (0.25f - isMoving) / 0.25f;
                transform.position = Vector3.Lerp(currentPos, nextPosition, complete);

            }

            if (bombTimer > 0)
                bombTimer -= Time.deltaTime;

            if (Input.GetKey(KeyCode.Space) && bombTimer <= 0 && !stats.amiDead()) {
                gc.PlaceBombInPoint(gc.myNumber, (int)Mathf.Round(transform.position.x), (int)Mathf.Round(transform.position.z));
                gamec.SendTheBombHasBeenPlanted((int)Mathf.Round(transform.position.x), (int)Mathf.Round(transform.position.z));
                bombTimer = 3.0f;
            }
        }
    }

}
