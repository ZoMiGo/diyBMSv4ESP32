#include "webserver_helper_funcs.h"

char CookieValue[20 + 1];
// DIYBMS=a&zp!b4okcj$2$Dg*zUC; path=/; HttpOnly; SameSite=Strict
char cookie[45 + sizeof(CookieValue)];

void setCookie(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Set-Cookie", cookie);
}

void setCookieValue()
{
    // We generate a unique number which is used in all following JSON requests
    // we use this as a simple method to avoid cross site scripting attacks
    // This MUST be done once the WIFI is switched on otherwise only PSEUDO random data is generated!!

    // ESP32 has inbuilt random number generator
    // https://techtutorialsx.com/2017/12/22/esp32-arduino-random-number-generation/

    // Pick random characters from this string (we could just use ASCII offset instead of this)
    // but this also avoids javascript escape characters like backslash and cookie escape chars like ; and %
    char alphabet[] = "!$*#@ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz";

    memset(CookieValue, 0, sizeof(CookieValue));

    // Leave NULL terminator on char array
    for (uint8_t x = 0; x < sizeof(CookieValue) - 1; x++)
    {
        // Random number between 0 and array length (minus null char)
        CookieValue[x] = alphabet[random(0, sizeof(alphabet) - 2)];
    }

    // Generate the full cookie string, as a HTTPONLY cookie, valid for this session only
    snprintf(cookie, sizeof(cookie), "DIYBMS=%s; path=/; HttpOnly; SameSite=Strict", CookieValue);
}

bool getPostDataIntoBuffer(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */

    if (req->content_len > sizeof(httpbuf))
    {
        ESP_LOGE(TAG, "Buffer not large enough %u", req->content_len);
        return false;
    }

    /* Truncate if content length larger than the buffer */
    // size_t recv_size = min(req->content_len, sizeof(httpbuf));

    int ret = httpd_req_recv(req, httpbuf, req->content_len);
    if (ret <= 0)
    { /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        // if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        //{
        /* In case of timeout one can choose to retry calling
         * httpd_req_recv(), but to keep it simple, here we
         * respond with an HTTP 408 (Request Timeout) error */
        // httpd_resp_send_408(req);
        // }
        return false;
    }

    // Ensure null terminated
    httpbuf[ret] = 0;

    // ESP_LOGD(TAG, "Post data %s", httpbuf);

    return true;
}

// Gets text value from a character buffer (as returned in HTTP request, query string etc)
bool GetTextFromKeyValue(const char *buffer, const char *key, char *text, size_t textLength, bool urlEncoded)
{
    if (httpd_query_key_value(httpbuf, key, text, textLength) == ESP_OK)
    {
        // ESP_LOGD(TAG, "Found: %s=%s", key, param);

        if (urlEncoded)
        {
            // Decode the incoming char array
            char *buf = (char *)malloc(textLength + 1);
            if (buf == NULL)
            {
                ESP_LOGE(TAG, "Unable to malloc");
                return false;
            }

            strncpy(buf, text, textLength);
            // Overwrite existing char array
            url_decode(buf, text);
            free(buf);
        }

        return true;
    }

    ESP_LOGW(TAG, "Missing key named '%s'", key);

    return false;
}


// Gets a FLOAT value from a character buffer (as returned in HTTP request, query string etc)
bool GetKeyValue(const char *buffer, const char *key, float *value, bool urlEncoded)
{
    char param[32];

    if (GetTextFromKeyValue(httpbuf, key, param, sizeof(param), urlEncoded))
    {
        // String to number conversion
        char **endptr = NULL;
        float v = strtof(param, endptr);

        *value = v;
        return true;
    }

    return false;
}


// Gets an unsigned long value from a character buffer (as returned in HTTP request, query string etc)
bool GetKeyValue(const char *buffer, const char *key, uint32_t *value, bool urlEncoded)
{
    char param[32];

    if (GetTextFromKeyValue(httpbuf, key, param, sizeof(param), urlEncoded))
    {
        // String to number conversion
        char **endptr = NULL;
        unsigned long v = strtoul(param, endptr, 10);

        *value = v;
        return true;
    }

    return false;
}

bool GetKeyValue(const char *buffer, const char *key, bool *value, bool urlEncoded)
{
    char param[32];

    if (GetTextFromKeyValue(httpbuf, key, param, sizeof(param), urlEncoded))
    {
        // Compare strings
        bool v = false;

        if (strncmp(param, "on", 2) == 0)
        {
            v = true;
        }

        *value = v;
        return true;
    }

    return false;
}

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
void url_decode(char *str, char *buf)
{

    // ESP_LOGD(TAG, "Encoded: %s", str);
    //*buf = malloc(strlen(str) + 1),
    char *pstr = str, *pbuf = buf;
    while (*pstr)
    {
        if (*pstr == '%')
        {
            if (pstr[1] && pstr[2])
            {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        }
        else if (*pstr == '+')
        {
            *pbuf++ = ' ';
        }
        else
        {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';

    ESP_LOGD(TAG, "Decoded: %s", buf);
}

/* Get the value of a cookie from the request headers */
esp_err_t httpd_req_get_cookie_val(httpd_req_t *req, const char *cookie_name, char *val, size_t *val_size)
{
    esp_err_t ret;
    size_t hdr_len_cookie = httpd_req_get_hdr_value_len(req, "Cookie");
    char *cookie_str = NULL;

    if (hdr_len_cookie <= 0)
    {
        return ESP_ERR_NOT_FOUND;
    }
    cookie_str = (char *)malloc(hdr_len_cookie + 1);
    if (cookie_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return ESP_ERR_NO_MEM;
    }

    if (httpd_req_get_hdr_value_str(req, "Cookie", cookie_str, hdr_len_cookie + 1) != ESP_OK)
    {
        ESP_LOGW(TAG, "Cookie not found");
        free(cookie_str);
        return ESP_ERR_NOT_FOUND;
    }

    ret = httpd_cookie_key_value(cookie_str, cookie_name, val, val_size);
    free(cookie_str);
    return ret;
}

/* Helper function to get a cookie value from a cookie string of the type "cookie1=val1; cookie2=val2" */
esp_err_t httpd_cookie_key_value(const char *cookie_str, const char *key, char *val, size_t *val_size)
{
    if (cookie_str == NULL || key == NULL || val == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    const char *cookie_ptr = cookie_str;
    const size_t buf_len = *val_size;
    size_t _val_size = *val_size;

    while (strlen(cookie_ptr))
    {
        /* Search for the '=' character. Else, it would mean
         * that the parameter is invalid */
        const char *val_ptr = strchr(cookie_ptr, '=');
        if (!val_ptr)
        {
            break;
        }
        size_t offset = val_ptr - cookie_ptr;

        /* If the key, does not match, continue searching.
         * Compare lengths first as key from cookie string is not
         * null terminated (has '=' in the end) */
        if ((offset != strlen(key)) || (strncasecmp(cookie_ptr, key, offset) != 0))
        {
            /* Get the name=val string. Multiple name=value pairs
             * are separated by '; ' */
            cookie_ptr = strchr(val_ptr, ' ');
            if (!cookie_ptr)
            {
                break;
            }
            cookie_ptr++;
            continue;
        }

        /* Locate start of next query */
        cookie_ptr = strchr(++val_ptr, ';');
        /* Or this could be the last query, in which
         * case get to the end of query string */
        if (!cookie_ptr)
        {
            cookie_ptr = val_ptr + strlen(val_ptr);
        }

        /* Update value length, including one byte for null */
        _val_size = cookie_ptr - val_ptr + 1;

        /* Copy value to the caller's buffer. */
        strlcpy(val, val_ptr, min(_val_size, buf_len));

        /* If buffer length is smaller than needed, return truncation error */
        if (buf_len < _val_size)
        {
            *val_size = _val_size;
            return ESP_ERR_HTTPD_RESULT_TRUNC;
        }
        /* Save amount of bytes copied to caller's buffer */
        *val_size = min(_val_size, buf_len);
        return ESP_OK;
    }
    ESP_LOGD(TAG, "cookie %s not found", key);
    return ESP_ERR_NOT_FOUND;
}

bool validateXSS(httpd_req_t *req)
{
    char requestcookie[sizeof(CookieValue)];

    size_t cookielength = sizeof(CookieValue);

    esp_err_t result = httpd_req_get_cookie_val(req, "DIYBMS", requestcookie, &cookielength);

    if (result == ESP_OK)
    {
        // Compare received cookie to our expected cookie
        if (strncmp(CookieValue, requestcookie, sizeof(CookieValue)) == 0)
        {
            // All good, everything ok.
            return true;
        }

        // Cookie found and returned correctly (not truncated etc)
        ESP_LOGW(TAG, "Incorrect cookie received %s", requestcookie);
    }

    // Fail - wrong cookie or not supplied etc.
    httpd_resp_send_err(req, httpd_err_code_t::HTTPD_400_BAD_REQUEST, "Invalid cookie");
    return false;
}

// Determine if the HTTPD request has a Content-Type of x-www-form-urlencoded
bool HasURLEncodedHeader(httpd_req_t *req)
{
    const char value[] = "application/x-www-form-urlencoded";
    char buffer[128];

    esp_err_t result = httpd_req_get_hdr_value_str(req, "Content-Type", buffer, sizeof(buffer));
    // ESP_LOGD(TAG, "esp_err_t=%i", result);

    if (result == ESP_OK)
    {
        // ESP_LOGD(TAG, "URLEncode=%s", buffer);
        //  Compare received value to our expected value (just the start of the string, minus null char)
        if (strncmp(value, buffer, sizeof(value) - 1) == 0)
        {
            // ESP_LOGD(TAG, "Header found and matches x-www-form-urlencoded");
            return true;
        }

        // Didn't match, so fall through to FALSE handler
    }

    return false;
}

bool validateXSSWithPOST(httpd_req_t *req, const char *postbuffer, bool urlencoded)
{
    // Need to validate POST variable as well...
    if (validateXSS(req))
    {
        // Must be larger than CookieValue as it could be URLEncoded
        char param[2 * sizeof(CookieValue)];
        if (httpd_query_key_value(postbuffer, "xss", param, sizeof(param)) == ESP_OK)
        {

            if (urlencoded)
            {
                // Decode the incoming char array
                char param_encoded[sizeof(param)];
                strncpy(param_encoded, param, sizeof(param));
                // Overwrite existing char array
                url_decode(param_encoded, param);
            }

            // Compare received cookie to our expected cookie
            if (strncmp(CookieValue, param, sizeof(CookieValue)) == 0)
            {
                // All good, everything ok.
                return true;
            }

            // Cookie found and returned correctly (not truncated etc)
            ESP_LOGW(TAG, "Incorrect POST cookie %s", param);
        }
        else
        {
            ESP_LOGW(TAG, "xss query key returned not OK");
        }
        // Failed POST XSS check
        httpd_resp_send_err(req, httpd_err_code_t::HTTPD_400_BAD_REQUEST, "Invalid cookie");
        return false;
    }

    // validateXSS has already sent httpd_resp_send_err...
    return false;
}