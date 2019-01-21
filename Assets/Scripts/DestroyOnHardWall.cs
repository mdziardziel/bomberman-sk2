using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestroyOnHardWall : MonoBehaviour {

    public GameObject x11;
    public GameObject x21;
    public GameObject z11;
    public GameObject z21;
    public GameObject x12;
    public GameObject x22;
    public GameObject z12;
    public GameObject z22;
    public int owningPlayer;

    private void Start() {
        GlobalController gc = GameObject.FindWithTag("GlobalController").GetComponent<GlobalController>();
        GameObject playerO = GameObject.FindWithTag("Player");
        PlayerController playc = playerO.GetComponent<PlayerController>();
        Transform player = playerO.transform;
       
        if ((int)Mathf.Round(transform.position.x) == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z) == (int)Mathf.Round(player.position.z))
            playc.MakeDead(owningPlayer);
        if ((int)Mathf.Round(transform.position.x)%2 == 0 ) {
            if ((int)Mathf.Round(transform.position.z) > 0) {
                if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x), (int)Mathf.Round(transform.position.z)-1)) {
                    z22.SetActive(true);
                    if ((int)Mathf.Round(transform.position.x) == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z) - 1 == (int)Mathf.Round(player.position.z))
                        playc.MakeDead(owningPlayer);
                    if ((int)Mathf.Round(transform.position.z) > 1)
                        if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x), (int)Mathf.Round(transform.position.z) - 2))
                        if ((int)Mathf.Round(transform.position.x) == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z)-2 == (int)Mathf.Round(player.position.z))
                            playc.MakeDead(owningPlayer);
                }
                z21.SetActive(true);

            }
            if ((int)Mathf.Round(transform.position.z) < gc.gameMap.getSize('y')) {

                if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x), (int)Mathf.Round(transform.position.z) + 1)) {
                    z12.SetActive(true);
                    if ((int)Mathf.Round(transform.position.x) == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z) + 1 == (int)Mathf.Round(player.position.z))
                        playc.MakeDead(owningPlayer);
                    if ((int)Mathf.Round(transform.position.z) < gc.gameMap.getSize('y')-1)
                        if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x), (int)Mathf.Round(transform.position.z) + 2))
                        if ((int)Mathf.Round(transform.position.x) == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z) + 2 == (int)Mathf.Round(player.position.z))
                            playc.MakeDead(owningPlayer);
                }
                z11.SetActive(true);

            }
           
        }
        if ((int)Mathf.Round(transform.position.z) % 2 == 0) {

            if ((int)Mathf.Round(transform.position.x) > 0) {
                if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x)-1, (int)Mathf.Round(transform.position.z) )) {
                    x22.SetActive(true);
                    if ((int)Mathf.Round(transform.position.x)-1 == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z)  == (int)Mathf.Round(player.position.z))
                        playc.MakeDead(owningPlayer);
                    if ((int)Mathf.Round(transform.position.x) > 1)
                        if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x)-2, (int)Mathf.Round(transform.position.z) ))
                        if ((int)Mathf.Round(transform.position.x)-2 == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z)  == (int)Mathf.Round(player.position.z))
                            playc.MakeDead(owningPlayer);
                }
                x21.SetActive(true);

            }

            if ((int)Mathf.Round(transform.position.x) < gc.gameMap.getSize('x')) {
                if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x) + 1, (int)Mathf.Round(transform.position.z))) {
                    x12.SetActive(true);
                    if ((int)Mathf.Round(transform.position.x) + 1 == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z) == (int)Mathf.Round(player.position.z))
                        playc.MakeDead(owningPlayer);
                    if ((int)Mathf.Round(transform.position.x) < gc.gameMap.getSize('x')-1)
                        if (!gc.gameMap.destroyIfSoft((int)Mathf.Round(transform.position.x) + 2, (int)Mathf.Round(transform.position.z)))
                        if ((int)Mathf.Round(transform.position.x) + 2 == (int)Mathf.Round(player.position.x) && (int)Mathf.Round(transform.position.z) == (int)Mathf.Round(player.position.z))
                            playc.MakeDead(owningPlayer);
                }
                x11.SetActive(true);

            }

        }



    }

 
}
