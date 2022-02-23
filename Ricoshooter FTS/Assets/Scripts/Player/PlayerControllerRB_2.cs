using System;
using UnityEngine;
using UnityEngine.SceneManagement;
using Mirror;
using TMPro;
using UnityEngine.UI;

public class PlayerControllerRB_2 : NetworkBehaviour
{
    //Assingables
    public Transform playerCam;
    public Transform orientation;
    public HealthbarScript healthBar;
    [SerializeField] GameObject headTarget;
    [SerializeField] Transform pistolRotationPoint;
    public GameObject ExitButton;
    private bool isPaused = false;
    private bool canMove = true;

    //Other
    private Rigidbody rb;
    [SyncVar]
    public string nickname;
    public TextMeshProUGUI nicknameText;
    public Canvas GUI;

    [Header("Health")]
    [SyncVar]
    [SerializeField] public float hp = 100f;
    public float respawnTime = 3f;


    //Rotation and look
    [Header("Look Parameters")]
    [SerializeField, Range(1, 10)] private float lookSpeedX = 2.0f;
    [SerializeField, Range(1, 10)] private float lookSpeedY = 2.0f;
    [SerializeField, Range(1, 180)] private float upperLookLimit = 80.0f;
    [SerializeField, Range(1, 180)] private float lowerLookLimit = 80.0f;
    private float rotationX = 0f;

    //Movement
    public float moveSpeed = 4500;
    public float maxSpeed = 20;
    public bool grounded;
    public LayerMask whatIsGround;

    public float counterMovement = 0.175f;
    private float threshold = 0.01f;
    public float maxSlopeAngle = 35f;

    //Crouch & Slide
    private Vector3 crouchScale = new Vector3(1, 0.5f, 1);
    private Vector3 playerScale;
    public float slideForce = 400;
    public float slideCounterMovement = 0.2f;

    //Jumping
    private bool readyToJump = true;
    private float jumpCooldown = 0.25f;
    public float jumpForce = 550f;

    //Input
    float x, y;
    bool jumping, sprinting, crouching;

    //Sliding
    private Vector3 normalVector = Vector3.up;
    private Vector3 wallNormalVector;


    //Animator
    private Animator animator;

    private bool Alive = true;
    private float timeToRespawn = 0f;

    void Awake()
    {
        rb = GetComponent<Rigidbody>();
        transform.position = GameObject.Find("/SpawnPoint").transform.position;
        
        //GetComponentInChildren<Canvas>().enabled = true;
        animator = GetComponentInChildren<Animator>();
        

    }

    void Start()
    {
        if (!isLocalPlayer)
        {
            Destroy(GetComponentInChildren<Camera>());
            Destroy(GetComponentInChildren<AudioListener>());
        }
        else
        {
            //healthBar = GetComponentInChildren<HealthbarScript>();
            GetComponentInChildren<SkinnedMeshRenderer>().shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.ShadowsOnly;
            healthBar.SetMaxHealth(hp);
            nickname = MenuButtonsScript.playerName;
            SetPlayerName(nickname);
        }
        playerScale = transform.localScale;
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;        
    }


    private void FixedUpdate()
    {
        if (isLocalPlayer && canMove)
        {
            Movement();
            animator.SetBool("IsRunning", Math.Abs(Input.GetAxisRaw("Horizontal")) > 0.2 || Math.Abs(Input.GetAxisRaw("Vertical")) > 0.2);
            animator.SetBool("IsJumping", !grounded);
        }

    }

    private void LateUpdate()
    {
        if (isLocalPlayer && canMove)
        {
            HandleMouseInput();
        }


    }

    private void Update()
    {
        if (isLocalPlayer)
        {
            if (Input.GetKeyDown(KeyCode.Escape))
            {
                canMove = !canMove;
                isPaused = !isPaused;
                //ExitButton.SetActive(isPaused);
                if (isPaused)
                {
                    Cursor.lockState = CursorLockMode.None;
                    Cursor.visible = true;
                }
                else
                {
                    Cursor.lockState = CursorLockMode.Locked;
                    Cursor.visible = false;
                }
            }

            if (!canMove)
            {
                return;
            }

            MyInput();
            if (Input.GetMouseButtonDown(0))
            {
                GunTrigger();
            }

            if (!Alive && Time.time >= timeToRespawn)
            {
                Respawn();
            }
            GunsriptUpdate();

            
        }
    }

    private void MyInput()
    {
        x = Input.GetAxisRaw("Horizontal");
        y = Input.GetAxisRaw("Vertical");
        jumping = Input.GetButton("Jump");
        crouching = Input.GetKey(KeyCode.LeftControl);

        //Crouching
        /*if (Input.GetKeyDown(KeyCode.LeftControl))
            StartCrouch();
        if (Input.GetKeyUp(KeyCode.LeftControl))
            StopCrouch();*/
    }

    /*private void StartCrouch()
    {
        transform.localScale = crouchScale;
        transform.position = new Vector3(transform.position.x, transform.position.y - 0.5f, transform.position.z);
        if (rb.velocity.magnitude > 0.5f)
        {
            if (grounded)
            {
                rb.AddForce(orientation.transform.forward * slideForce);
            }
        }
    }

    private void StopCrouch()
    {
        transform.localScale = playerScale;
        transform.position = new Vector3(transform.position.x, transform.position.y + 0.5f, transform.position.z);
    }*/

    private void Movement()
    {
        //Extra gravity
        rb.AddForce(Vector3.down * Time.deltaTime * 10);

        //Find actual velocity relative to where player is looking
        Vector2 mag = FindVelRelativeToLook();
        float xMag = mag.x, yMag = mag.y;

        //Counteract sliding and sloppy movement
        CounterMovement(x, y, mag);

        //If holding jump && ready to jump, then jump
        if (readyToJump && jumping) Jump();

        //Set max speed
        float maxSpeed = this.maxSpeed;

        //If sliding down a ramp, add force down so player stays grounded and also builds speed
        if (crouching && grounded && readyToJump)
        {
            rb.AddForce(Vector3.down * Time.deltaTime * 3000);
            return;
        }

        //If speed is larger than maxspeed, cancel out the input so you don't go over max speed
        if (x > 0 && xMag > maxSpeed) x = 0;
        if (x < 0 && xMag < -maxSpeed) x = 0;
        if (y > 0 && yMag > maxSpeed) y = 0;
        if (y < 0 && yMag < -maxSpeed) y = 0;

        //Some multipliers
        float multiplier = 1f, multiplierV = 1f;

        // Movement in air
        if (!grounded)
        {
            multiplier = 0.5f;
            multiplierV = 0.5f;
        }

        // Movement while sliding
        if (grounded && crouching) multiplierV = 0f;

        //Apply forces to move player
        rb.AddForce(orientation.transform.forward * y * moveSpeed * Time.deltaTime * multiplier * multiplierV);
        rb.AddForce(orientation.transform.right * x * moveSpeed * Time.deltaTime * multiplier);
    }

    private void Jump()
    {
        if (grounded && readyToJump)
        {
            readyToJump = false;

            //Add jump forces
            rb.AddForce(Vector2.up * jumpForce * 1.5f);
            rb.AddForce(normalVector * jumpForce * 0.5f);

            //If jumping while falling, reset y velocity.
            Vector3 vel = rb.velocity;
            if (rb.velocity.y < 0.5f)
                rb.velocity = new Vector3(vel.x, 0, vel.z);
            else if (rb.velocity.y > 0)
                rb.velocity = new Vector3(vel.x, vel.y / 2, vel.z);

            Invoke(nameof(ResetJump), jumpCooldown);
        }
    }

    private void ResetJump()
    {
        readyToJump = true;
    }

    private void HandleMouseInput()
    {
        
        rotationX -= Input.GetAxis("Mouse Y") * lookSpeedY;
        rb.MoveRotation(rb.rotation * Quaternion.Euler(new Vector3(0, Input.GetAxis("Mouse X") * lookSpeedX, 0)));
        
        rotationX = Mathf.Clamp(rotationX, -upperLookLimit, lowerLookLimit);

        GetComponentInChildren<Camera>().transform.localRotation = Quaternion.Euler(rotationX, 0, 0);
        
        headTarget.transform.position = GetComponentInChildren<Camera>().transform.position + GetComponentInChildren<Camera>().transform.forward * 5f;  
        
        pistolRotationPoint.localRotation = Quaternion.Euler(rotationX, 0, 0);
    }

    private void CounterMovement(float x, float y, Vector2 mag)
    {
        if (!grounded || jumping) return;

        //Slow down sliding
        if (crouching)
        {
            rb.AddForce(moveSpeed * Time.deltaTime * -rb.velocity.normalized * slideCounterMovement);
            return;
        }

        //Counter movement
        if (Math.Abs(mag.x) > threshold && Math.Abs(x) < 0.05f || (mag.x < -threshold && x > 0) || (mag.x > threshold && x < 0))
        {
            rb.AddForce(moveSpeed * orientation.transform.right * Time.deltaTime * -mag.x * counterMovement);
        }
        if (Math.Abs(mag.y) > threshold && Math.Abs(y) < 0.05f || (mag.y < -threshold && y > 0) || (mag.y > threshold && y < 0))
        {
            rb.AddForce(moveSpeed * orientation.transform.forward * Time.deltaTime * -mag.y * counterMovement);
        }

        //Limit diagonal running. This will also cause a full stop if sliding fast and un-crouching, so not optimal.
        if (Mathf.Sqrt((Mathf.Pow(rb.velocity.x, 2) + Mathf.Pow(rb.velocity.z, 2))) > maxSpeed)
        {
            float fallspeed = rb.velocity.y;
            Vector3 n = rb.velocity.normalized * maxSpeed;
            rb.velocity = new Vector3(n.x, fallspeed, n.z);
        }
    }

    public Vector2 FindVelRelativeToLook()
    {
        float lookAngle = orientation.transform.eulerAngles.y;
        float moveAngle = Mathf.Atan2(rb.velocity.x, rb.velocity.z) * Mathf.Rad2Deg;

        float u = Mathf.DeltaAngle(lookAngle, moveAngle);
        float v = 90 - u;

        float magnitue = rb.velocity.magnitude;
        float yMag = magnitue * Mathf.Cos(u * Mathf.Deg2Rad);
        float xMag = magnitue * Mathf.Cos(v * Mathf.Deg2Rad);

        return new Vector2(xMag, yMag);
    }

    private bool IsFloor(Vector3 v)
    {
        float angle = Vector3.Angle(Vector3.up, v);
        return angle < maxSlopeAngle;
    }

    private bool cancellingGrounded;

    private void OnCollisionStay(Collision other)
    {
        //Make sure we are only checking for walkable layers
        int layer = other.gameObject.layer;
        if (whatIsGround != (whatIsGround | (1 << layer))) return;

        //Iterate through every collision in a physics update
        for (int i = 0; i < other.contactCount; i++)
        {
            Vector3 normal = other.contacts[i].normal;
            //FLOOR
            if (IsFloor(normal))
            {
                grounded = true;
                cancellingGrounded = false;
                normalVector = normal;
                CancelInvoke(nameof(StopGrounded));
            }
        }

        float delay = 3f;
        if (!cancellingGrounded)
        {
            cancellingGrounded = true;
            Invoke(nameof(StopGrounded), Time.deltaTime * delay);
        }
    }

    private void StopGrounded()
    {
        grounded = false;
    }  

    [Command]
    void GunTrigger()
    {
        GetComponentInChildren<GunScript>().Shoot();
    }

    [ClientRpc]
    public void TakeDamage(float damage)
    {
        if (hp >= 0 && isLocalPlayer)
        {
            hp -= damage;
            UpdateHealthbar(hp);
            if (hp <= 0)
            {
                Die();
            }
        }
        
    }

    void Die()
    {
        if (isLocalPlayer)
        {
            timeToRespawn = Time.time + respawnTime;
            Alive = false;
            GetComponentInChildren<Camera>().enabled = false;
            transform.position = GameObject.Find("/DeadPlatform/DieSpawn").transform.position;
            DisableGUI();
        }
    }

    void Respawn()
    {
        if (isLocalPlayer)
        {
            GetComponentInChildren<Camera>().enabled = true;
            Alive = true;
            hp = 100f;
            healthBar.SetMaxHealth(100);
            GetComponentInChildren<GunScript>().currentAmmo = GetComponentInChildren<GunScript>().maxAmmo;
            transform.position = GameObject.Find("/SpawnPoint").transform.position;
            EnableGUI();
        }
    }

    public void UpdateHealthbar(float health)
    {
        healthBar.SetHealth(health);
    }

    [Command]
    void GunsriptUpdate()
    {
        GetComponentInChildren<GunScript>().UpdateAmmoText();
    }

    void SetPlayerName(string name)
    {
        nicknameText.text = name;
    }

    void EnableGUI()
    {
        GUI.enabled = true;
    }
  
    void DisableGUI()
    {
        GUI.enabled = false;
    }

    public void Disconnect()
    {
        Debug.Log("Pressed");
        NetworkManager.singleton.StopClient();
        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    private void OnDisconnectedFromServer()
    {
        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;
    }

    
}